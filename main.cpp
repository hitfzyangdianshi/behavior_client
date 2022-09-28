#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <ctime>
#include <sys/time.h>



#include "utils.h"


using namespace std;




int main(int argc, char **argv) {

    FILE *challenge_call;
    int pid;
    double proc_cpu;
    int proc_mem, virtual_proc_mem;

    challenge_call = popen("./challenge_test 1 2", "r");


    pid = get_pid("DPI_challenge");

    printf("pid = %d\n", pid);

    for(int i=1;i<=20;i++) {
        proc_cpu = get_proc_cpu(pid);
        printf("proc_cpu = %f\n", proc_cpu);
    }

    proc_mem= get_proc_mem(pid);
    printf("proc_mem = %d\n", proc_mem);

//    virtual_proc_mem=get_proc_virtualmem(pid);
//    printf("virtual_proc_mem = %d\n", virtual_proc_mem);

    return 0;
}



