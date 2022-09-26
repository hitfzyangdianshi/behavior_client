//
// Created by qwertyu on 9/21/22.
//

#include "cup_utilization.h"

using namespace std;



//ref: https://developer.aliyun.com/article/837483
double cal_cpuoccupy (cpu_occupy_t *o, cpu_occupy_t *n)
{
    double od, nd;
    double id, sd;
    double cpu_use ;

    od = (double) (o->user + o->nice + o->system +o->idle+o->softirq+o->iowait+o->irq);//1
    nd = (double) (n->user + n->nice + n->system +n->idle+n->softirq+n->iowait+n->irq);//2

    id = (double) (n->idle);    //user
    sd = (double) (o->idle) ;    //system
    if((nd-od) != 0)
        cpu_use =100.0 - ((id-sd))/(nd-od)*100.00; //((user+system)*100)/(time difference) -> g_cpu_used
    else
        cpu_use = 0;
    return cpu_use;
}


void get_cpuoccupy (cpu_occupy_t *cpust)
{
    FILE *fd;
    int n;
    char buff[256];
    cpu_occupy_t *cpu_occupy;
    cpu_occupy=cpust;

    fd = fopen ("/proc/stat", "r");
    if(fd == NULL)
    {
        perror("cpu utilization fopen:");
        exit (-1);
    }
    fgets (buff, sizeof(buff), fd);

    sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle ,&cpu_occupy->iowait,&cpu_occupy->irq,&cpu_occupy->softirq);

    fclose(fd);
}


/*
double get_sysCpuUsage()
{
    cpu_occupy_t cpu_stat1;
    cpu_occupy_t cpu_stat2;
    double cpu;
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat1);
    sleep(1);
    //第二次获取cpu使用情况
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat2);

    //计算cpu使用率
    cpu = cal_cpuoccupy ((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);

    return cpu;
}*/
