#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <ctime>
#include <sys/time.h>



#include "cup_utilization.h"
#include "memory_utilization.h"

using namespace std;




int main(int argc, char **argv) {

    for(auto i=1;i<=9;i++) {

        cpu_occupy_t cpu_stat1;
        cpu_occupy_t cpu_stat2;
        double cpu;

        get_cpuoccupy((cpu_occupy_t *)&cpu_stat1);
        clock_t time1=clock();

        FILE *challenge_call;


        //sleep(1);
        challenge_call= popen("./challenge_test 1 2","r");

        clock_t time2=clock();
        double memory_utilization=  get_memory_utilization();

        get_cpuoccupy((cpu_occupy_t *)&cpu_stat2);


        cpu = cal_cpuoccupy ((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);

        double execution = (double)((time2-time1) *1000000.0/CLOCKS_PER_SEC );

        printf("[%d]\tCPU utilization:%f\tmemory utilization:%f\texecurtion time(ms):%f\n",i,cpu,memory_utilization,execution);

        /*char challenge_result[80];
        fgets(challenge_result, sizeof (challenge_result),challenge_call);
        printf("%s\n",challenge_result);*/
    }


    return 0;
}



