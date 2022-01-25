#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include "UiGpuItem.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UiGpuItem;
class ServerGpuWin;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void InitHistoryServers();
    void AddNewServer(const QString& strServerIp);

private slots:
    void on_ckbRtLine_toggled(bool checked);

    void on_btnAddServer_clicked();

    void on_btnRemoveServer_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* m_pTimer;
    UiGpuItem* m_pItem;
    std::vector<UiGpuItem*> vecGpuItem;

    QMap<int, std::shared_ptr<ServerGpuWin>> m_mapGpuWinPos;
};
#endif // MAINWINDOW_H
