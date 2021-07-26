#include "GpuMonitor.h"
#include "nvml.h"
#include <iostream>

GpuMonitor* GpuMonitor::inst = nullptr;
std::mutex GpuMonitor::m_mtx;

const char* convertToComputeModeString(nvmlComputeMode_t mode)
{
    switch (mode)
    {
    case NVML_COMPUTEMODE_DEFAULT:
        return "Default";
    case NVML_COMPUTEMODE_EXCLUSIVE_THREAD:
        return "Exclusive_Thread";
    case NVML_COMPUTEMODE_PROHIBITED:
        return "Prohibited";
    case NVML_COMPUTEMODE_EXCLUSIVE_PROCESS:
        return "Exclusive Process";
    default:
        return "Unknown";
    }
}

GpuMonitor::GpuMonitor():
    m_aiSelGpuId(0)
{
    init();
}

GpuMonitor::~GpuMonitor()
{
    Release();
}

bool GpuMonitor::init()
{
    nvmlReturn_t result = nvmlInit();
    if (NVML_SUCCESS != result)
    {
        std::cerr << "Failed to initialize NVML: " << nvmlErrorString(result) << std::endl;
        return false;
    }

    return true;
}

GpuMonitor* GpuMonitor::GetInstance()
{
    if (nullptr == inst)
    {
        std::lock_guard<std::mutex> locker(m_mtx);
        if (nullptr == inst)
        {
            inst = new GpuMonitor;
        }
    }
    return inst;
}

void GpuMonitor::SetSelGpuId(int iSelId)
{
    m_aiSelGpuId.store(iSelId);
}

unsigned long long GpuMonitor::GetCurGpuRestMem()
{
    std::vector<gpu_mem_info> vecGpuInfo;
    if (GetGpuInfo(vecGpuInfo))
    {
        return vecGpuInfo[m_aiSelGpuId.load()].llFree;
    }

    return 0;
}

void GpuMonitor::Release()
{
    nvmlReturn_t result = nvmlShutdown();
    if (NVML_SUCCESS != result)
    {
        std::cout << "Failed to shutdown NVML: " << nvmlErrorString(result) << std::endl;
    }
}

bool GpuMonitor::GetGpuInfo(std::vector<gpu_mem_info>& vecGpuInfo)
{
    unsigned int device_count;
    nvmlReturn_t result = nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != result)
    {
        std::cout << "Failed to query device count: " << nvmlErrorString(result) << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < device_count; i++)
    {
        nvmlDevice_t device;
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (NVML_SUCCESS != result)
        {
            std::cerr << "Failed to get handle for device " << i << ": " << nvmlErrorString(result) << std::endl;
            continue;
        }

        nvmlMemory_t memory;
        result = nvmlDeviceGetMemoryInfo(device, &memory);
        if (NVML_SUCCESS != result)
        {
            std::cerr << "Failed to get handle for device " << i << ": " << nvmlErrorString(result) << std::endl;
            continue;
        }
        
        char strName[NVML_DEVICE_NAME_BUFFER_SIZE] = {0};
        nvmlDeviceGetName(device, strName, NVML_DEVICE_NAME_BUFFER_SIZE);
        vecGpuInfo.push_back({ i, std::string(strName), memory.used, memory.free, memory.total});
    }

    return !vecGpuInfo.empty();
}
