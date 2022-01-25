#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GpuMonitor.h"
#include <QSpacerItem>
#include "ServerGpuWin.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    //this->setWindowOpacity(0.85);

    m_pItem = new UiGpuItem(this);
    ui->gridLayout->addWidget(m_pItem);

    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(500);

    connect(m_pTimer, &QTimer::timeout, this, [&](){
        std::vector<gpu_mem_info> vecGpuInfo;
        GpuMonitor::GetInstance()->GetGpuInfo(vecGpuInfo);

        if (ui->cmbGpu->count() == 0)
        {
            for (auto gpu : vecGpuInfo) {
                ui->cmbGpu->addItem(QString::number(gpu.id) + " " + QString::fromStdString(gpu.strName), gpu.id);
            }
        }

        for (size_t i=0; i<vecGpuInfo.size(); ++i)
        {
            if (ui->cmbGpu->currentData(Qt::UserRole).toUInt() == vecGpuInfo[i].id)
            {
                m_pItem->SetGpuInfo(vecGpuInfo[i]);
                break;
            }
        }
    });

    m_pTimer->start();

    InitHistoryServers();
}

MainWindow::~MainWindow()
{
    QSettings setting("config.ini", QSettings::IniFormat);
    setting.beginGroup("main");
    QStringList lstServerIp;
    for (auto it=m_mapGpuWinPos.begin(); it!=m_mapGpuWinPos.end(); ++it)
    {
        lstServerIp << it.value()->GetServerIp();
    }

    setting.setValue("servers", lstServerIp);
    setting.endGroup();

    delete ui;
}

void MainWindow::InitHistoryServers()
{
    QSettings setting("config.ini", QSettings::IniFormat);
    setting.beginGroup("main");
    QStringList lstServerIp = setting.value("servers").toStringList();
    setting.endGroup();

    foreach(auto& str, lstServerIp)
    {
        AddNewServer(str);
    }
}


void MainWindow::on_ckbRtLine_toggled(bool checked)
{
    m_pItem->SetRtLineVisible(checked);

    if (checked){
        this->setMaximumHeight(300);
        this->setMinimumHeight(300);
    }
    else {
        this->setMaximumHeight(80);
        this->setMinimumHeight(80);
    }

    this->adjustSize();
}

void MainWindow::AddNewServer(const QString& strServerIp)
{
    auto spGpuWin = std::make_shared<ServerGpuWin>(this, strServerIp);

    connect(spGpuWin.get(), &ServerGpuWin::sigServerIp, this, [&](const QString& strServerIp){
        ServerGpuWin* p = qobject_cast<ServerGpuWin*>(sender());

        for (auto it=m_mapGpuWinPos.begin(); it!=m_mapGpuWinPos.end(); ++it)
        {
            if (it.value().get() == p)
            {
                ui->tabWidget->setTabText(it.key(), strServerIp);
                return;
            }
        }
    });
    connect(spGpuWin.get(), &ServerGpuWin::sigGpuUsedRate, this, [&](const QString& strGpuUsedRate){
        ServerGpuWin* p = qobject_cast<ServerGpuWin*>(sender());

        for (auto it=m_mapGpuWinPos.begin(); it!=m_mapGpuWinPos.end(); ++it)
        {
            if (it.value().get() == p)
            {
                ui->tabWidget->setTabText(it.key(), p->GetServerIp()+strGpuUsedRate);
                return;
            }
        }
    });

    int idx = ui->tabWidget->addTab(spGpuWin.get(), strServerIp.isEmpty()?"unknown server":strServerIp);
    ui->tabWidget->setCurrentIndex(idx);

    m_mapGpuWinPos.insert(idx, spGpuWin);
}

void MainWindow::on_btnAddServer_clicked()
{
    AddNewServer("");
}

void MainWindow::on_btnRemoveServer_clicked()
{
    int idx = ui->tabWidget->currentIndex();
    if (idx > 0)
    {
        ui->tabWidget->removeTab(idx);
    }

    if(m_mapGpuWinPos.contains(idx))
    {
        m_mapGpuWinPos.remove(idx);
    }
}
