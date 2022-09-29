#include <unistd.h>
#include <sys/time.h>
#include <bits/stdc++.h>

#include "utils.h"


using namespace std;


void * run_challenge(void *pVoid){
    char *buffer;
    buffer = (char *)malloc(9999*sizeof(char));
    FILE *fp;
    fp=popen("../../DPI_challenge/cmake-build-debug/DPI_challenge 77785013981551582343293798510443924959819057315170007596898940617969596087533115026951499894039902248846791609082584911446550364291013377086145727196953287853703181523044693184006168499541635734974518338177127986068424623677884947462862567494290279550993930991636393774544224102456139692653784144946384674721399141364811253440183811656861558720345735944803400260533998259914078019082187647176761313535767485202646237839574347186337095641076426343761905828352269566830703817878713983333711383742746209161506947321366421127198006811720502000160","r");
    //fgets(buffer, sizeof(buffer),fp);
    char c;
    int i=0;
    while ( (c=fgetc(fp))!=EOF ){
        buffer[i]=c;
        i++;
    }
    return (void*) buffer;
}

void * getcpu(void *pVoid){
    FILE *output_cpu=fopen("output_cpu.txt","w");
    int pid = 0;
    while(pid==0){
        pid = get_pid("DPI_challenge");
    }
    char pid_path[64]={0};
    sprintf(pid_path,"/proc/%d/stat",pid);
    double proc_cpu;
    printf("pid = %d\n", pid);
    while(true){
        if(fopen(pid_path,"r")==NULL){
            printf("get_cpu ends\n");
            break;
        }
        proc_cpu = get_proc_cpu(pid);
        printf("proc_cpu = %f\n", proc_cpu);
        if(proc_cpu<100){
            fprintf(output_cpu,"%f\n",proc_cpu);
        }
    }
}

void * getmem(void *pVoid){

}

void * server_send(void *pVoid){

}

void * server_receiver(void *pVoid){

}


const char composite_number[]="77785013981551582343293798510443924959819057315170007596898940617969596087533115026951499894039902248846791609082584911446550364291013377086145727196953287853703181523044693184006168499541635734974518338177127986068424623677884947462862567494290279550993930991636393774544224102456139692653784144946384674721399141364811253440183811656861558720345735944803400260533998259914078019082187647176761313535767485202646237839574347186337095641076426343761905828352269566830703817878713983333711383742746209161506947321366421127198006811720502000160";

int main(int argc, char **argv) {

    pthread_t thread_challenge;
    pthread_t thread_getcpu;
    //pthread_t thread_getmem;
    //pthread_t thread_gettime;


    void *ret_challenge;

    pthread_create(&thread_challenge,NULL, run_challenge,NULL);
    pthread_create(&thread_getcpu, NULL, getcpu, NULL );
    //pthread_create(&thread_getmem, NULL, getmem, NULL );

    pthread_join(thread_challenge, &ret_challenge);
    pthread_join(thread_getcpu, NULL);
    //pthread_join(thread_getmem, NULL);

    for(auto i=0;i<9999;i++){
        if( ((char*)ret_challenge)[i]!='\0')    printf("%c",((char*)ret_challenge)[i]);
        else break;
    }

#ifdef PRINT_TEST_MAIN
    int pid;
    double proc_cpu;
    int proc_mem, virtual_proc_mem;

    pid = get_pid("DPI_challenge");

    printf("pid = %d\n", pid);

    for(int i=1;i<=20;i++) {
        proc_cpu = get_proc_cpu(pid);
        printf("proc_cpu = %f\n", proc_cpu);
    }

    proc_mem= get_proc_mem(pid);
    printf("proc_mem = %d\n", proc_mem);

    virtual_proc_mem=get_proc_virtualmem(pid);
    printf("virtual_proc_mem = %d\n", virtual_proc_mem);
#endif

    return 0;
}



