#ifndef UIGPUITEM_H
#define UIGPUITEM_H

#include <QWidget>
#include "gpu_info.h"
#include "qcustomplot.h"

namespace Ui {
class UiGpuItem;
}

class UiGpuItem : public QWidget
{
    Q_OBJECT

public:
    explicit UiGpuItem(QWidget *parent = nullptr);
    ~UiGpuItem();

    void SetGpuInfo(gpu_mem_info info);

    void SetRtLineVisible(bool bVisible);

private:
    Ui::UiGpuItem *ui;
    QCustomPlot* m_pCustomPlot;
};

#endif // UIGPUITEM_H
