#include "ServerGpuWin.h"
#include "ui_ServerGpuWin.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QMap>
#include <nlohmann/json.hpp>
#include <QSet>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>

using json = nlohmann::json;

ServerGpuWin::ServerGpuWin(QWidget *parent, const QString& strServerIp) :
    QWidget(parent),
    ui(new Ui::ServerGpuWin),
    m_pNetWork(nullptr),
    m_pTimer(nullptr)
{
    ui->setupUi(this);
    ui->table->horizontalHeader()->setStretchLastSection(true);

    m_pNetWork = new QNetworkAccessManager(this);
    connect(m_pNetWork, &QNetworkAccessManager::finished, this, &ServerGpuWin::sltUpdateGpuInfo);

    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(1000);
    connect(m_pTimer, &QTimer::timeout, this, [&]{

        QString strIp = ui->edtIp->text().trimmed();
        QString strPort = ui->edtPort->text().trimmed();
        QString strUrl = QString("http://%1:%2/gpuinfo").arg(strIp).arg(strPort);

        QNetworkRequest request;
        request.setUrl(QUrl(strUrl));
        request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

        m_pNetWork->get(QNetworkRequest(request));
    });

    if (!strServerIp.isEmpty())
    {
        ui->edtIp->setText(strServerIp);

        on_btnUpdate_clicked();
    }
}

ServerGpuWin::~ServerGpuWin()
{
    delete ui;
}

QString ServerGpuWin::GetServerIp()
{
    return ui->edtIp->text();
}

void ServerGpuWin::UpdateUi(const QMap<int, GpuInfo> &mapGpuInfo)
{
    if (mapGpuInfo.empty()) return;

    int iGpuId = -1;

    QList<int> lstIds = mapGpuInfo.keys();
    int iCnt = ui->cmbGpuId->count();
    if (iCnt == 0 || iCnt != lstIds.size())
    {
        ui->cmbGpuId->clear();

        foreach(auto id, lstIds)
        {
            ui->cmbGpuId->addItem(QString::number(id));
        }

        ui->cmbGpuId->setCurrentIndex(0);
        iGpuId = ui->cmbGpuId->currentText().toInt();
    }
    else
    {
        QList<int> lst;
        for (int i=0; i<iCnt; ++i) {
            lst.push_back(ui->cmbGpuId->itemText(i).toInt());
        }

        if (lst == lstIds)
        {
            iGpuId = ui->cmbGpuId->currentText().toInt();
        }
        else
        {
            ui->cmbGpuId->clear();

            foreach(auto id, lstIds)
            {
                ui->cmbGpuId->addItem(QString::number(id));
            }

            ui->cmbGpuId->setCurrentIndex(0);
            iGpuId = ui->cmbGpuId->currentText().toInt();
        }
    }

    if (iGpuId == -1) return;

    GpuInfo info = mapGpuInfo[iGpuId];
    ui->lbName->setText(info.strName);
    ui->lbTemp->setText(QString::number(info.iTemp));
    ui->lbFree->setText(QString::number(info.iFree/(1024.0*1024.0), 'f', 2));
    ui->lbUsed->setText(QString::number(info.iUsed/(1024.0*1024.0), 'f', 2));
    ui->lbTotal->setText(QString::number(info.iTotal/(1024.0*1024.0), 'f', 2));
    ui->lbRate->setText(QString::number(info.fRate*100.0, 'f', 2));

    ui->table->clearContents();
    ui->table->setRowCount(info.vecProcess.size());
    int iRow = 0;
    for (auto p : info.vecProcess) {
        QTableWidgetItem* item0 = new QTableWidgetItem(QString::number(p.pid));
        item0->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        ui->table->setItem(iRow, 0, item0);

        QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(p.iUsedMem/(1024.0*1024.0)));
        item1->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        ui->table->setItem(iRow, 1, item1);

        QTableWidgetItem* item2 = new QTableWidgetItem(QString::number(p.iHostMem));
        item2->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        ui->table->setItem(iRow, 2, item2);

        QTableWidgetItem* item3 = new QTableWidgetItem(QString::number(p.fCpuRate*100, 'f', 2));
        item3->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        ui->table->setItem(iRow, 3, item3);

        QTableWidgetItem* item4 = new QTableWidgetItem(p.strUser);
        item4->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        ui->table->setItem(iRow, 4, item4);

        QTableWidgetItem* item5 = new QTableWidgetItem(p.strName);
        ui->table->setItem(iRow, 5, item5);

        iRow++;
    }

    ui->lbUpdateTime->setText(QTime::currentTime().toString("hh:mm:ss"));

    sigGpuUsedRate("(" + QString::number(info.fRate*100.0, 'f', 0) + "%)");
}

void ServerGpuWin::on_btnUpdate_clicked()
{
    if(m_pTimer->isActive())
    {
        m_pTimer->stop();
        ui->btnUpdate->setText("update");
    }
    else
    {
        m_pTimer->start();
        ui->btnUpdate->setText("stop");

        emit sigServerIp(ui->edtIp->text());
    }
}

void ServerGpuWin::sltUpdateGpuInfo(QNetworkReply *reply)
{
    QByteArray baJson = reply->readAll();
    reply->deleteLater();

    if (baJson.isEmpty()) return;

    QMap<int, GpuInfo> mapGpuInfo;
    try {

        QJsonDocument jsonDoc = QJsonDocument::fromJson(baJson);
        QJsonArray jGpus = jsonDoc["gpus"].toArray();
        for (auto it=jGpus.begin(); it!= jGpus.end(); ++it)
        {
            QJsonValue jGpu = *it;
            GpuInfo info;

            info.id = jGpu["id"].toInt();
            info.strName = jGpu["name"].toString();
            info.iTemp = jGpu["temperature"].toInt();

            info.iFree = jGpu["free"].toVariant().toULongLong();
            info.iUsed = jGpu["used"].toVariant().toULongLong();
            info.iTotal = jGpu["total"].toVariant().toULongLong();
            info.fRate = jGpu["used_rate"].toDouble();

            if (jGpu["processes"].isNull())
            {
                mapGpuInfo.insert(info.id, info);
                continue;
            }

            QJsonArray jProcesses = jGpu["processes"].toArray();
            for (auto iter=jProcesses.begin(); iter!=jProcesses.end(); ++iter)
            {
                ProcessInfo p;
                QJsonValue jProcess = *iter;
                p.strName = jProcess["name"].toString();
                p.pid = jProcess["pid"].toVariant().toUInt();
                p.iUsedMem = jProcess["device_memory"].toVariant().toULongLong();

                p.iHostMem = jProcess["host_memory"].toVariant().toULongLong();
                p.fCpuRate = jProcess["host_cpu"].toVariant().toFloat();

                p.strUser = jProcess["user"].toVariant().toString();

                info.vecProcess.push_back(p);
            }

            mapGpuInfo.insert(info.id, info);
        }

        UpdateUi(mapGpuInfo);
    } catch (std::exception& e) {
        qDebug() << e.what();
    }
}
