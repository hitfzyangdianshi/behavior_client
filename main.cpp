#include <unistd.h>
#include <bits/stdc++.h>

#include<cerrno>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "utils.h"


#define MAXLINE 4096


using namespace std;

long execution_time;
struct timeval tv1, tv2;

char composite_number[]="777850139815515823432937985104439249598190573151700075968989406179695960875331150269514998940399022488467916090825849114465503642910133770861457271969532878537031815230446931840061684995416357349745183381771279860684246236778849474628625674942902795509939309916363937745442241024561396926537841449463846747213991413648112534401838116568615587203457359448034002605339982599140780190821876471767613135357674";

void * run_challenge(void *composite_number_void){

    char *composite_number_char;
    composite_number_char=(char*)composite_number_void;

    char *buffer;
    buffer = (char *)malloc(9999*sizeof(char));
    FILE *fp;

    char filename[1000];
    sprintf(filename,"../../DPI_challenge/cmake-build-debug/DPI_challenge %s",composite_number_char);

   // fp=popen("../../DPI_challenge/cmake-build-debug/DPI_challenge 7778501398155158234329379851044392495981905731517000759689894061796959608753311502695149989403990224884679160908258491144655036429101337708614572719695328785370318152304469318400616849954163573497451833817712798606842462367788494746286256749429027955099393099163639377454422410245613969265378414494638467472139914136481125344018381165686155872034573594480340026053399825991407801908218764717676131353576748520264623783957434718633709564107642634376190582835226956683070381787871398","r");
    fp= popen(filename,"r");

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
#ifdef PRINT_DEBUG_INFO
        printf("proc_cpu = %f\n", proc_cpu);
#endif
        if(proc_cpu<100){
            fprintf(output_cpu,"%f\n",proc_cpu);
        }
    }
    fclose(output_cpu);
}

void * getmem(void *pVoid){
    int pid = 0;
    while(pid==0){
        pid = get_pid("DPI_challenge");
    }

    usleep(500);

    unsigned int proc_mem, virtual_proc_mem;
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
bool client_send(const char *ip, int port, FILE *filepath_to_send){
    int   sockfd, n;
    char  recvline[4096], sendline[4096];
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

    fgets(sendline, 4096, stdin);
    if( send(sockfd, sendline, strlen(sendline), 0) < 0){
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }

    close(sockfd);
    return true;
}

char* client_receiver(int port){
    char * received_composite_number;
    received_composite_number=(char *)malloc(9999*sizeof(char));
    int  listenfd, connfd,             n;
    struct sockaddr_in  servaddr;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return nullptr;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return nullptr;
    }

    if( listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return nullptr;
    }

    printf("====== waiting for composite_number ======\n");

    while(true){
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        n = recv(connfd, received_composite_number, MAXLINE, 0);
        received_composite_number[n] = '\0';
        printf("receive composite_number: %s\n", received_composite_number);
        break;
    }

    return received_composite_number;
}



int main(int argc, char **argv) {

    pthread_t thread_challenge;
    pthread_t thread_getcpu;
    pthread_t thread_getmem;
    pthread_t thread_execurtion_time;


    void *ret_challenge;

    pthread_create(&thread_challenge,NULL, run_challenge,composite_number);
    pthread_create(&thread_getcpu, NULL, getcpu, NULL );
    pthread_create(&thread_getmem, NULL, getmem, NULL );
    pthread_create(&thread_execurtion_time,NULL,get_execurtion_timne,NULL);

    pthread_join(thread_challenge, &ret_challenge);
    pthread_join(thread_getcpu, NULL);
    pthread_join(thread_getmem, NULL);
    pthread_join(thread_execurtion_time,NULL);

    for(auto i=0;i<9999;i++){
        if( ((char*)ret_challenge)[i]!='\0')    printf("%c",((char*)ret_challenge)[i]);
        else break;
    }
    printf("\n");

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


    FILE *cpu_txt=fopen("output_cpu.txt","r");
    double previous_cpu_average=0, current_cpu;
    for(auto i=1;;i++){
        if(fscanf(cpu_txt,"%lf",&current_cpu)==EOF){
            break;
        }
        previous_cpu_average=(previous_cpu_average*(i-1)+current_cpu)/((double)i);
    }

    printf("average cpu usage: %f\n", previous_cpu_average);
    printf("execution time 10e-6 seconds: %ld\n", execution_time);


    return 0;
}