//
// Created by qwertyu on 9/21/22.
//

#include "memory_utilization.h"

using namespace std;


// (total - available) / total * 100
double get_memory_utilization()
{
    FILE *fp;
    fp= fopen("/proc/meminfo","r");
    if(fp == NULL)
    {
        perror("memory utilization fopen:");
        exit (-1);
    }

    mem_info_t mem_info;

    fscanf(fp,"%s %lu %s %s %lu %s %s %lu %s",mem_info.mem_total_name,&mem_info.mem_total_,mem_info.mem_total_unit,mem_info.mem_free_name,&mem_info.mem_free_,mem_info.mem_free_unit,mem_info.mem_available_name,&mem_info.mem_available_,mem_info.mem_free_unit);

    double mem;
    mem=((mem_info.mem_total_-mem_info.mem_available_)*100.0)/mem_info.mem_total_;

    return mem;

}