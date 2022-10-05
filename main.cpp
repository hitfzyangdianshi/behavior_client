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

char composite_number[1000];

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
    buffer[i+1]='\0';

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
        proc_cpu = get_proc_cpu(pid);

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


void * get_execurtion_timne(void *pVoid){
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




//socket ref: https://www.cnblogs.com/zkfopen/p/9441264.html
bool client_send(const char *ip, int port, char *buffer_to_send){
    int   sockfd, n;
    struct sockaddr_in  servaddr;

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        return false;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",ip);
        return false;
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        return false;
    }


    if( send(sockfd, buffer_to_send, strlen(buffer_to_send), 0) < 0){
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }

    close(sockfd);
    return true;
}

void client_receiver(int port){
    int  listenfd, connfd,             n;
    struct sockaddr_in  servaddr;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return ;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return ;
    }

    if( listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return ;
    }



    struct sockaddr_in addr_client;
    unsigned int len = sizeof(sockaddr_in);


    printf("====== waiting for composite_number ======\n");

    if ( (connfd = accept(listenfd, ((sockaddr *) &addr_client), &len)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
        return ;
    }

    n=recv(connfd, composite_number, 9999, 0);
    composite_number[n] = '\0';
    printf("receive composite_number[%d]: %s\n", n, composite_number);
    //for(int i=0;i<9999;i++)printf("%c",received_composite_number[i]);

    printf("accept ip address: %s\n", inet_ntoa(addr_client.sin_addr) );

    close(listenfd);
    close(connfd);
}




int main(int argc, char **argv) {


    memset(buffer,'\0',sizeof(char)*2000);

    client_receiver(4321);



    pthread_t thread_challenge;
    pthread_t thread_getcpu;
    pthread_t thread_getmem;
    pthread_t thread_execurtion_time;



    pthread_create(&thread_challenge,NULL, run_challenge,NULL);
    pthread_create(&thread_getcpu, NULL, getcpu, NULL );
    pthread_create(&thread_getmem, NULL, getmem, NULL );
    pthread_create(&thread_execurtion_time,NULL,get_execurtion_timne,NULL);

    pthread_join(thread_challenge, NULL);
    pthread_join(thread_getcpu, NULL);
    pthread_join(thread_getmem, NULL);
    pthread_join(thread_execurtion_time,NULL);


    printf("pthread ends . \n");



    FILE *cpu_txt=fopen("output_cpu.txt","r");
    double previous_cpu_average=0, current_cpu;
    for(int i=1;;i++){
        if(fscanf(cpu_txt,"%lf",&current_cpu)==EOF){
            break;
        }
        previous_cpu_average=(previous_cpu_average*(i-1)+current_cpu)/((double)i);
    }

    printf("average cpu usage: %f\n", previous_cpu_average);


    char buffer1[3000];
    sprintf(buffer1,"%s\n*\nVMSIZE= %d\nRSS= %d\nCPU_usage= %f\nexecurtion_time(10e-6s)= %ld\n@",buffer,virtual_proc_mem,proc_mem,previous_cpu_average,execution_time);


    bool send_data=client_send(argv[1],4321,buffer1);
    printf("send back: %s\n",btoa(send_data));

    return 0;
}