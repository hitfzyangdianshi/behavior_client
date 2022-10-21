//
// Created by qwertyu on 9/26/22.
//

#include "utils.h"

using namespace std;


//获取第N项开始的指针 get the pointer to the start of the Nth item
const char* get_items(const char*buffer ,unsigned int item){

    const char *p =buffer;

    int len = strlen(buffer);
    int count = 0;

    for (int i=0; i<len;i++){
        if (' ' == *p){
            count ++;
            if(count == item -1){
                p++;
                break;
            }
        }
        p++;
    }

    return p;
}



//获取总的CPU时间  get the total CPU time
unsigned long get_cpu_total_occupy(){

    FILE *fd;
    char buff[1024]={0};
    Total_Cpu_Occupy_t t;

    fd =fopen("/proc/stat","r");
    if (nullptr == fd){
        return 0;
    }

    fgets(buff,sizeof(buff),fd);
    char name[64]={0};
    sscanf(buff,"%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
    fclose(fd);
#ifdef PRINT_DEBUG_INFO
    printf("cpu total: %s %ld %ld %ld %ld  ",name,t.user,t.nice,t.system,t.idle);
#endif
    return (t.user + t.nice + t.system + t.idle);
}



//获取进程的CPU时间   get the CPU time of a process
unsigned long get_cpu_proc_occupy(unsigned int pid){

    char file_name[64]={0};
    Proc_Cpu_Occupy_t t;
    FILE *fd;
    char line_buff[1024]={0};
    sprintf(file_name,"/proc/%d/stat",pid);

    fd = fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    fgets(line_buff,sizeof(line_buff),fd);

    sscanf(line_buff,"%u",&t.pid);
    const char *q =get_items(line_buff,PROCESS_ITEM);
    sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
    fclose(fd);
#ifdef PRINT_DEBUG_INFO
    printf("proc: %ld %ld %ld %ld \n",t.utime,t.stime,t.cutime,t.cstime);
#endif
    return (t.utime + t.stime + t.cutime + t.cstime);
}




//获取CPU占用率    get CPU utilization
float get_proc_cpu(unsigned int pid, __useconds_t sleep_us) {

    unsigned long totalcputime1, totalcputime2;
    unsigned long procputime1, procputime2;

    totalcputime1 = get_cpu_total_occupy();
    procputime1 = get_cpu_proc_occupy(pid);

    usleep(sleep_us);

    totalcputime2 = get_cpu_total_occupy();
    procputime2 = get_cpu_proc_occupy(pid);

    float pcpu = 0.0;
    if (0 != totalcputime2 - totalcputime1) {
        pcpu = 100.0 * (procputime2 - procputime1) / (totalcputime2 - totalcputime1);
    }

    return pcpu;
}




//获取进程占用内存    get process memory usage
unsigned int get_proc_mem(unsigned int pid){

    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    sprintf(file_name,"/proc/%d/status",pid);

    fd =fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    char name[64];
    int vmrss;
    for (int i=0; i<VMRSS_LINE-1;i++){
        fgets(line_buff,sizeof(line_buff),fd);
    }

    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%s %d",name,&vmrss);
#ifdef PRINT_DEBUG_INFO
    printf("proc mem: %s %d\n",name,vmrss);
#endif
    fclose(fd);

    return vmrss;
}





//获取进程占用虚拟内存   get the virtual memory occupied by the process
unsigned int get_proc_virtualmem(unsigned int pid){

    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    sprintf(file_name,"/proc/%d/status",pid);

    fd =fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    char name[64];
    int vmsize;
    for (int i=0; i<VMSIZE_LINE-1;i++){
        fgets(line_buff,sizeof(line_buff),fd);
    }

    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%s %d",name,&vmsize);
#ifdef PRINT_DEBUG_INFO
    printf("proc virtual mem: %s %d\n",name,vmsize);
#endif
    fclose(fd);

    return vmsize;
}



//进程本身 get PID
int get_pid(const char* process_name, const char* user  )
{
    if(user == nullptr){
        user = getlogin();
    }

    char cmd[512];
    if (user){
        sprintf(cmd, "pgrep %s -u %s", process_name, user);
    }

    FILE *pstr = popen(cmd,"r");

    if(pstr == nullptr){
        return 0;
    }

    char buff[512];
    ::memset(buff, 0, sizeof(buff));
    if(NULL == fgets(buff, 512, pstr)){
        return 0;
    }

    return atoi(buff);
}
