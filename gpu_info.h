#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <string>

struct gpu_mem_info
{
    unsigned int id;
    std::string strName;
    unsigned long long llUsed;
    unsigned long long llFree;
    unsigned long long llTotal;
	
    gpu_mem_info(unsigned int i, std::string name, unsigned long long used, unsigned long long free, unsigned long long total):
    id(i), strName(name), llUsed(used), llFree(free), llTotal(total)
	{
		
	}
	
	gpu_mem_info():
    id(0), llUsed(0), llFree(0), llTotal(0)
	{
		
	}
};

#endif
