#include "UiGpuItem.h"
#include "ui_UiGpuItem.h"

UiGpuItem::UiGpuItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UiGpuItem),
    m_pCustomPlot(nullptr)
{
    ui->setupUi(this);

    if (nullptr == m_pCustomPlot)
    {
        m_pCustomPlot = new QCustomPlot(this);
        m_pCustomPlot->addGraph(); // blue line
        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(2);
        m_pCustomPlot->graph(0)->setPen(pen);

        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        m_pCustomPlot->xAxis->setTicker(timeTicker);
        m_pCustomPlot->axisRect()->setupFullAxesBox();
        m_pCustomPlot->yAxis->setRange(0, 4096);

        // make left and bottom axes transfer their ranges to right and top axes:
        connect(m_pCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_pCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(m_pCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_pCustomPlot->yAxis2, SLOT(setRange(QCPRange)));

        ui->grid->addWidget(m_pCustomPlot);
    }
}

UiGpuItem::~UiGpuItem()
{
    delete ui;
}

void UiGpuItem::SetGpuInfo(gpu_mem_info info)
{
    ui->lbId->setText(QString::number(info.id));
    ui->lbName->setText(QString::fromStdString(info.strName));
    ui->lbFree->setText(QString::number(info.llFree/1024.0/1024.0));
    ui->lbUsed->setText(QString::number(info.llUsed/1024.0/1024.0));
    ui->lbTotal->setText(QString::number(info.llTotal/1024.0/1024.0));

    static qreal x = 0;
    if (x > 120)
    {
        x=0;
    }

    if (ui->widget->isVisible())
    {
        static QTime timeStart = QTime::currentTime();
        // calculate two new data points:
        double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds

        if (info.llUsed/(info.llTotal*1.0) > 0.9)
        {
            QPen pen = m_pCustomPlot->graph(0)->pen();
            pen.setColor(Qt::red);
            m_pCustomPlot->graph(0)->setPen(pen);
        }
        else
        {
            QPen pen = m_pCustomPlot->graph(0)->pen();
            pen.setColor(Qt::blue);
            m_pCustomPlot->graph(0)->setPen(pen);
        }

        m_pCustomPlot->graph(0)->addData(key, info.llUsed/1024.0/1024.0);

        // make key axis range scroll with the data (at a constant range size of 8):
        m_pCustomPlot->xAxis->setRange(key, 60, Qt::AlignRight);
        m_pCustomPlot->yAxis->setRange(0, info.llTotal/1024.0/1024.0);
        m_pCustomPlot->replot();
    }
}

void UiGpuItem::SetRtLineVisible(bool bVisible)
{
    ui->widget->setVisible(bVisible);
}
