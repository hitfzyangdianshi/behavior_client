//
// Created by qwertyu on 9/21/22.
//

#ifndef UNTITLED_CUP_UTILIZATION_H
#define UNTITLED_CUP_UTILIZATION_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>



class cup_utilization {

};

typedef struct cpu_occupy_
{
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
    unsigned int steal;
    unsigned int guest;
    unsigned int guest_nice;
}cpu_occupy_t;


double cal_cpuoccupy (cpu_occupy_t *o, cpu_occupy_t *n);

void get_cpuoccupy (cpu_occupy_t *cpust);



//double get_sysCpuUsage();



#endif //UNTITLED_CUP_UTILIZATION_H
