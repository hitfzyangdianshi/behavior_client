//
// Created by qwertyu on 9/26/22.
//

#ifndef UTILS_H
#define UTILS_H

#define HAVE_NULLPTR // remove this if you do not have nullptr in g++

#ifndef HAVE_NULLPTR
#if defined(_WIN64)
# define nullptr 0LL
#elif defined(__GNUC__)
# define nullptr __null
#else
# define nullptr 0L
#endif
#endif /* defined(HAVE_NULLPTR) */

#include <unistd.h>
#include <sys/time.h>
#include <bits/stdc++.h>


/*please confirm the line number of /proc/{pid}/status before compiling */
#define VMRSS_LINE 22
#define VMSIZE_LINE 18
#define PROCESS_ITEM 14

class utils {

};


typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
}Total_Cpu_Occupy_t;


typedef struct {
    unsigned int pid;
    unsigned long utime;  //user time
    unsigned long stime;  //kernel time
    unsigned long cutime; //all user time
    unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;



//获取CPU占用率    get CPU utilization
float get_proc_cpu(unsigned int pid, __useconds_t sleep_us=20000);

//获取进程占用内存    get process memory usage
unsigned int get_proc_mem(unsigned int pid);


//获取进程占用虚拟内存    get the virtual memory occupied by the process
unsigned int get_proc_virtualmem(unsigned int pid);

//进程本身 get PID
int get_pid(const char* process_name, const char* user = NULL);

#endif //UTILS_H
