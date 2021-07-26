#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GpuMonitor.h"
#include <QSpacerItem>

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
}

MainWindow::~MainWindow()
{
    delete ui;
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
