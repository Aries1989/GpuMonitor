#ifndef SERVERGPUWIN_H
#define SERVERGPUWIN_H

#include <QWidget>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVector>

struct ProcessInfo
{
    QString strName;
    int pid;
    size_t iUsedMem;
    size_t iHostMem;
    float fCpuRate;

    QString strUser;
};

struct GpuInfo
{
    int id;

    QString strName;
    int iTemp;

    size_t iFree;
    size_t iUsed;
    size_t iTotal;
    float fRate;

    QVector<ProcessInfo> vecProcess;
};

namespace Ui {
class ServerGpuWin;
}

class ServerGpuWin : public QWidget
{
    Q_OBJECT

public:
    explicit ServerGpuWin(QWidget *parent = nullptr, const QString& strServerIp="");
    ~ServerGpuWin();

    QString GetServerIp();

signals:
    void sigServerIp(const QString& strServerIp);
    void sigGpuUsedRate(const QString& strGpuUsedRate);

private:
    void UpdateUi(const QMap<int, GpuInfo>& mapGpuInfo);

private slots:
    void on_btnUpdate_clicked();

    void sltUpdateGpuInfo(QNetworkReply *reply);

private:
    Ui::ServerGpuWin *ui;

    QNetworkAccessManager* m_pNetWork;
    QTimer* m_pTimer;
};

#endif // SERVERGPUWIN_H
