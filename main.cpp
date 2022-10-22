#include <unistd.h>
#include <bits/stdc++.h>

#include<cerrno>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "utils.h"

#define btoa(x) ((x)?"true":"false")

using namespace std;

long execution_time;
struct timeval tv1, tv2;

//char composite_number[]="777850139815515823432937985104439249598190573151700075968989406179695960875331150269514998940399022488467916090825849114465503642910133770861457271969532878537031815230446931840061684995416357349745183381771279860684246236778849474628625674942902795509939309916363937745442241024561396926537841449463846747213991413648112534401838116568615587203457359448034002605339982599140780190821876471767613135357674";


char buffer[2000];

char composite_number[]="7778501398155158234329379851044392495981905731517000759689894061796959608753311502695149989403990224884679160908258491144655036429101337708614572719695328785370318152304469318400616849954";


// using /usr/bin/time
void get_execurtion_info(char *composite_number_char){

    char filename[1000];
    sprintf(filename,"../../DPI_challenge/cmake-build-debug/DPI_challenge %s",composite_number_char);

    char _usr_bin_time[]=R"(/usr/bin/time -f "\n*\nrealtime %e\nmemory_kb %M\ncpu %P\n@"  2>&1)";

    char _usr_bin_time_command[1300];
    sprintf(_usr_bin_time_command, "%s %s",_usr_bin_time,filename);

    FILE *fp;
    fp= popen(_usr_bin_time_command,"r");

    char c;
    int i=0;
    while ( (c=fgetc(fp))!=EOF ){
        buffer[i]=c;
        if(buffer[i]=='@')break;
        i++;
        if(i>=2000)break;
    }

    pclose(fp);
}








// using utils.cpp
void * run_challenge(void *pVoid){

    FILE *fp;

    char filename[1000];
    sprintf(filename,"../../DPI_challenge/cmake-build-debug/DPI_challenge %s",composite_number);



    fp= popen(filename,"r");


    //fgets(buffer, sizeof(buffer),fp);
    char c;
    int i=0;
    while ( (c=fgetc(fp))!=EOF ){
        buffer[i]=c;
        if(buffer[i]==255 || buffer[i]==EOF || buffer[i]==0)break; //solve segmentation fault issue, because the end of the reading fp is all 255
        i++;
    }
    buffer[i]='\0';

    for( i=0;i<2000;i++){
        if( buffer[i]!='\0')    printf("%c",buffer[i]);
        else break;
    }

}




void * getcpu(void *pVoid){
    FILE *output_cpu=fopen("output_cpu.txt","w");
    int pid = 0;
    while(pid==0){
        pid = get_pid("DPI_challenge");
        printf("pid = %d\n", pid);
    }


    char pid_path[64]={0};
    sprintf(pid_path,"/proc/%d/stat",pid);
    double proc_cpu;


    while(true){
        if(fopen(pid_path,"r")==NULL){

            printf("get_cpu ends\n");
            break;
        }
        proc_cpu = get_proc_cpu(pid,5000);

        if(proc_cpu<=100){
            fprintf(output_cpu,"%f\n",proc_cpu);
        }
    }
    fclose(output_cpu);
}



unsigned int proc_mem, virtual_proc_mem;
void * getmem(void *pVoid){
    int pid = 0;
    while(pid==0){
        pid = get_pid("DPI_challenge");
    }

    usleep(500);


    proc_mem= get_proc_mem(pid);
    printf("proc_mem = %d\n", proc_mem);

    virtual_proc_mem=get_proc_virtualmem(pid);
    printf("virtual_proc_mem = %d\n", virtual_proc_mem);

    FILE *out_mem=fopen("getmem.txt","w");
    fprintf(out_mem,"proc_mem= %d\n", proc_mem);
    fprintf(out_mem,"virtual_proc_mem= %d\n", virtual_proc_mem);
    fclose(out_mem);
}


void * get_execurtion_time(void *pVoid){
    int pid = 0;
    while(pid==0){
        pid = get_pid("DPI_challenge");
    }
    gettimeofday(&tv1, NULL);
    while(pid!=0){
        pid = get_pid("DPI_challenge");
    }
    gettimeofday(&tv2, NULL);

    execution_time=(tv2.tv_sec-tv1.tv_sec )* 1000000 + tv2.tv_usec-tv1.tv_usec;
}



void using_strace(char *composite_number_char){
    char cm[1000];
    //sprintf(cm,"sudo strace -p %d -v -d -f -C -o strace_result.txt",pid_c);
    sprintf(cm,"sudo strace -v -d -f -C -o strace_result.txt  ../../DPI_challenge/cmake-build-debug/DPI_challenge %s",composite_number_char);

    FILE *fp;
    fp= popen(cm,"r");

    fclose(fp);
}



int main(int argc, char **argv) {


    memset(buffer,'\0',sizeof(char)*2000);





    pthread_t thread_challenge;
    pthread_t thread_getcpu;
    pthread_t thread_getmem;
    pthread_t thread_execurtion_time;
 //   pthread_t thread_strace;



    pthread_create(&thread_challenge,NULL, run_challenge,NULL);
    pthread_create(&thread_getcpu, NULL, getcpu, NULL );
    pthread_create(&thread_getmem, NULL, getmem, NULL );
    pthread_create(&thread_execurtion_time,NULL,get_execurtion_time,NULL);
 //   pthread_create(&thread_strace,NULL,using_strace, NULL);

    pthread_join(thread_challenge, NULL);
    pthread_join(thread_getcpu, NULL);
    pthread_join(thread_getmem, NULL);
    pthread_join(thread_execurtion_time,NULL);
 //   pthread_join(thread_strace, NULL);


    printf("pthread ends . \n");



    using_strace(composite_number);



    FILE *cpu_txt=fopen("output_cpu.txt","r");
    double previous_cpu_average=0, current_cpu;
    for(int i=1;;i++){
        if(fscanf(cpu_txt,"%lf",&current_cpu)==EOF){
            break;
        }
        previous_cpu_average=(previous_cpu_average*(i-1)+current_cpu)/((double)i);
    }

    printf("average cpu usage: %f\n", previous_cpu_average);
    printf("execution_time: %ld\n", execution_time);


    //char buffer1[3000];
    //sprintf(buffer1,"%s\n*\nVMSIZE= %d\nRSS= %d\nCPU_usage= %f\nexecurtion_time(10e-6s)= %ld\n@",buffer,virtual_proc_mem,proc_mem,previous_cpu_average,execution_time);

    FILE* usage_out = fopen("usage_sample_out.txt", "a");
    fprintf(usage_out, "%f,%d,%d,%ld\n", previous_cpu_average, virtual_proc_mem, proc_mem, execution_time);
    fclose(usage_out);


    /***************************************************/
    memset(buffer,'\0',sizeof(char)*2000);
    get_execurtion_info(composite_number);

    FILE* write_output2 = fopen("reveive output2 out.txt", "w");

    for(int i=0;i<2000;i++){
        if( buffer[i]!='\0' && buffer[i]!='@')
        {
            printf("%c",buffer[i]);
            fprintf(write_output2,"%c",buffer[i]);
        }
        else {
            buffer[i]='\0';
            break;
        }
    }
    printf("\n");
    fclose(write_output2);


    FILE* read_output2 = fopen("reveive output2 out.txt", "r");
    char received_line[200];
    int memory2, cpupercentage;
    float realtime;
    for (int i = 1; i <= 26; i++) {
        if (i >= 1 && i <= 21) {
            fscanf(read_output2, "%s", received_line);
        } else if (i == 22) {
            fscanf(read_output2, "%s %f", received_line, &realtime);
        } else if (i == 23) {
            fscanf(read_output2, "%s %d", received_line, &memory2);
        } else if (i == 24) {
            fscanf(read_output2, "%s %d%%", received_line, &cpupercentage);
        } else {
            break;
        }
    }
    FILE* usage_out1 = fopen("usage_2_out.txt", "a");
    fprintf(usage_out1, "%d,%d,%.2f\n", cpupercentage, memory2, realtime);


    return 0;
}