#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include "gpu_info.h"
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>

class GpuMonitor
{
public:
    static GpuMonitor* GetInstance();

    void SetSelGpuId(int iSelId);

    unsigned long long GetCurGpuRestMem();

    bool GetGpuInfo(std::vector<gpu_mem_info>& vecGpuInfo);

private:
    GpuMonitor();
    ~GpuMonitor();

    bool init();
    void Release();

private:
    static GpuMonitor* inst;
    static std::mutex m_mtx;

    std::atomic<unsigned int> m_aiSelGpuId;
};

static bool CheckGpuMemEnough(unsigned long long mem_need)
{
    unsigned long long iRestMem = GpuMonitor::GetInstance()->GetCurGpuRestMem();
    if (iRestMem < mem_need)
    {
        std::cout << "there are no enough gpu memory for allocating(M): " << iRestMem << " < " << mem_need << std::endl;
        return false;
    }
    return true;
}

#endif
