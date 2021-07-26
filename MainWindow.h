#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include "UiGpuItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UiGpuItem;
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ckbRtLine_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QTimer* m_pTimer;
    UiGpuItem* m_pItem;
    std::vector<UiGpuItem*> vecGpuItem;
};
#endif // MAINWINDOW_H
