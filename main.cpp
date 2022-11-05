#include <unistd.h>
#include <bits/stdc++.h>

#include<sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

// using openssl-1.1.1q
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <syscall.h>
#include <sys/ptrace.h>

#include "utils.h"

#define btoa(x) ((x)?"true":"false")

#define DEFINE_ON_ARM (defined (__arm__) || defined(__arm64__) || defined(__aarch64__) || defined(RPI) || defined(__Raspberry_PI__) || defined(RASPBERRY_PI) || defined(__ARM_ARCH) )
#define DEFINE_RPI (defined(RPI) || defined(__Raspberry_PI__) || defined(RASPBERRY_PI)  )

#define FATAL(...) \
    do { \
        fprintf(stderr, "STrace: " __VA_ARGS__); \
        fputc('\n', stderr); \
        return; \
    } while (0)


using namespace std;


int port=4321;


long execution_time;
struct timeval tv1, tv2;

//char composite_number[]="777850139815515823432937985104439249598190573151700075968989406179695960875331150269514998940399022488467916090825849114465503642910133770861457271969532878537031815230446931840061684995416357349745183381771279860684246236778849474628625674942902795509939309916363937745442241024561396926537841449463846747213991413648112534401838116568615587203457359448034002605339982599140780190821876471767613135357674";


char buffer[40000];

char composite_number[1000];

char filename[1500];

#if !DEFINE_ON_ARM
#define FILENAME_PATH_S "../../DPI_challenge/cmake-build-debug/DPI_challenge %s"
#define CACERT "../keys/cacert.pem"
#define CLIENTCERT "../keys/clientcert.pem"
#define CLIENTKEY "../keys/clientkey.pem"
#else
#define FILENAME_PATH_S "../DPI_challenge-interposition/DPI_challenge %s"
#define CACERT "keys/cacert.pem"
#define CLIENTCERT "keys/clientcert.pem"
#define CLIENTKEY "keys/clientkey.pem"
#endif

void * run_challenge(void *pVoid){

    FILE *fp;

    sprintf(filename,FILENAME_PATH_S,composite_number);

    memset(buffer,'\0',sizeof(char)*40000);

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

    for( i=0;i<40000;i++){
        if( buffer[i]!='\0')    printf("%c",buffer[i]);
        else break;
    }

}



double cupaverage;
long cputag;
void * getcpu(void *pVoid){
    cupaverage=0;
    cputag=0;

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
        proc_cpu = get_proc_cpu(pid, 5000);

        if(proc_cpu<=100){
            fprintf(output_cpu,"%f\n",proc_cpu);
            if(cputag==0){
                cupaverage=proc_cpu;
                cputag++;
            }
            else{
                cupaverage=cupaverage/(cputag+1);
                cupaverage=cupaverage*cputag;
                cupaverage=cupaverage+proc_cpu/(cputag+1);
                cputag++;
            }
        }
    }
    fclose(output_cpu);
    printf("cputag= %ld\n",cputag);
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








char accepted_server_address[30];

int client_receiver(int port) {
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    if (listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    return listenfd;
}



int ptrace_count[1000000];
char ptrace_send[100000];
#if !DEFINE_ON_ARM
#define USING_PTRACE() using_ptrace_ubuntu64()
void using_ptrace_ubuntu64(){
    memset(ptrace_count,0,sizeof(ptrace_count[0])*1000000);
    memset(ptrace_send,'\0',sizeof(ptrace_send[0])*100000);

    pid_t pid = fork();

    switch (pid) {
        case -1: /* error */
            FATAL("%s", strerror(errno));
        case 0:  /* child */
            ptrace(PTRACE_TRACEME, 0, 0, 0);
            /* Because we're now a tracee, execvp will block until the parent
             * attaches and allows us to continue. */
            char challenge_path[]="../../DPI_challenge/cmake-build-debug/DPI_challenge";
            char *args[]={challenge_path,composite_number, NULL};
            execvp(challenge_path,args);
            FATAL("%s", strerror(errno));
    }

    /* parent */
    waitpid(pid, 0, 0); // sync with execvp
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);

    for (;;) {
        /* Enter next system call */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));
        if (waitpid(pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));

        /* Gather system call arguments */
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)
            FATAL("%s", strerror(errno));



        long syscall_ = (long)regs.orig_rax;

        /* Print a representation of the system call */
        // if(syscall_!=-1)
        fprintf(stderr, "%ld(%ld, %ld, %ld, %ld, %ld, %ld)",
                syscall_,
                (long)regs.rdi, (long)regs.rsi, (long)regs.rdx,
                (long)regs.r10, (long)regs.r8,  (long)regs.r9);


        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));
        if (waitpid(pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));

        /* Get system call result */
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1) {

            // fputs(" = ?\n", stderr);
            if (errno == ESRCH && !(syscall_ == SYS_exit || syscall_ == SYS_exit_group)) {
                // exit(regs.rdi); // system call was _exit(2) or similar
                FATAL("%s, %ld", strerror(errno), (long) regs.rdi);
            }
            if(!(syscall_ == SYS_exit || syscall_ == SYS_exit_group))
                FATAL("%s", strerror(errno));
        }

        /* Print system call result */
        fprintf(stderr, " = %ld\n", (long)regs.rax);
        if(syscall_!=-1) {
            ptrace_count[syscall_]++;
        }

        if(syscall_ == -1){
            fprintf(stderr, "[ERROR] orig_rax is -1\n");
            return;
            break;
        }
        else if(syscall_ == SYS_exit || syscall_ == SYS_exit_group){
            if((long)regs.rdi==0) {
                break;
            }
            else{
                fprintf(stderr, "[INFO] challenge exit code is %ld\n",(long)regs.rdi);
                break;
            }
        }
    }

    char ptrace_line[20];

    for(long i=0;i<1000000;i++){
        if(ptrace_count[i]>0){
            printf("%ld\t%d\n", i, ptrace_count[i]);
            sprintf(ptrace_line,"%ld\t%d\n", i, ptrace_count[i]);
            strcat(ptrace_send,ptrace_line);
        }
    }
}

#else// !DEFINE_ON_ARM
#if !DEFINE_RPI
#define USING_PTRACE() using_ptrace_arm64()
#include <sys/uio.h>
#include <linux/elf.h>
void using_ptrace_arm64(){  // test on jetsonTX2
    memset(ptrace_count,0,sizeof(ptrace_count[0])*1000000);
    memset(ptrace_send,'\0',sizeof(ptrace_send[0])*100000);

    pid_t pid = fork();
    // printf("pid = %d\n",pid);
    switch (pid) {
        case -1: /* error */
            FATAL("%s", strerror(errno));
        case 0:  /* child */
            ptrace(PTRACE_TRACEME, 0, 0, 0);
            /* Because we're now a tracee, execvp will block until the parent
             * attaches and allows us to continue. */
            char challenge_path[]="../DPI_challenge-interposition/DPI_challenge";
            char *args[]={challenge_path,composite_number, NULL};
            execvp(challenge_path,args);
            FATAL("[error execvp] %s", strerror(errno));
    }


    /* parent */
    waitpid(pid, 0, 0); // sync with execvp
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);

    for (;;) {
        /* Enter next system call */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            FATAL("[error PTRACE_SYSCALL1] %s", strerror(errno));
        if (waitpid(pid, 0, 0) == -1)
            FATAL("[error waitpid1] %s", strerror(errno));


        /* Gather system call arguments */
        struct user_regs_struct regs;
        struct iovec io;
        io.iov_base = &regs;
        io.iov_len = sizeof(regs);

        if (ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &io) == -1)
            FATAL("[error PTRACE_GETREGSET] %s", strerror(errno));


        long syscall_ = (long)regs.regs[8];

        /* Print a representation of the system call */
        // if(syscall_!=-1)
        fprintf(stderr, "%ld(%ld, %ld, %ld, %ld, %ld, %ld)",
                syscall_,
                (long)regs.regs[0], (long)regs.regs[1], (long)regs.regs[2],
                (long)regs.regs[3], (long)regs.regs[4],  (long)regs.regs[5]);


        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            FATAL("[error PTRACE_SYSCALL2] %s", strerror(errno));
        if (waitpid(pid, 0, 0) == -1)
            FATAL("[error waitpid2] %s", strerror(errno));

        /* Get system call result */
        if (ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &io) == -1) {
            //fputs(" = ?\n", stderr);
            if (errno == ESRCH && !(syscall_ == SYS_exit || syscall_ == SYS_exit_group)){
                // exit((long)regs.regs[0]); // system call was _exit(2) or similar
                FATAL("%s, %ld", strerror(errno),(long)regs.regs[0]);
            }
            if(!(syscall_ == SYS_exit || syscall_ == SYS_exit_group))
                FATAL("%s", strerror(errno));
        }

        /* Print system call result */
        fprintf(stderr, " = %ld\n", (long)regs.regs[0]);
        if(syscall_!=-1) {
            ptrace_count[syscall_]++;
        }

        if(syscall_ == -1){
            fprintf(stderr, "[ERROR] regs.regs[8] is -1\n");
            // exit(EXIT_FAILURE);
            return;
            break;
        }
        else if(syscall_ == SYS_exit || syscall_ == SYS_exit_group){
            if((long)regs.regs[0]==0) {
                break;
            }
            else{
                fprintf(stderr, "[INFO] challenge exit code is %ld\n",(long)regs.regs[0]);
                break;
            }
        }
    }


    char ptrace_line[20];

    for(long i=0;i<1000000;i++){
        if(ptrace_count[i]>0){
            printf("%ld\t%d\n", i, ptrace_count[i]);
            sprintf(ptrace_line,"%ld\t%d\n", i, ptrace_count[i]);
            strcat(ptrace_send,ptrace_line);
        }
    }
}


#else // !DEFINE_RPI
#define USING_PTRACE() using_ptrace_rpi32()
#include <sys/uio.h>
#include <linux/elf.h>
void using_ptrace_rpi32() { // Raspberry Pi pi_armv7l_32
    memset(ptrace_count,0,sizeof(ptrace_count[0])*1000000);
    memset(ptrace_send,'\0',sizeof(ptrace_send[0])*100000);

    pid_t pid = fork();
    // printf("pid = %d\n",pid);
    switch (pid) {
        case -1: /* error */
            FATAL("%s", strerror(errno));
        case 0:  /* child */
            ptrace(PTRACE_TRACEME, 0, 0, 0);
            /* Because we're now a tracee, execvp will block until the parent
             * attaches and allows us to continue. */
            char challenge_path[]="../DPI_challenge-interposition/DPI_challenge";
            char *args[]={challenge_path,composite_number, NULL};
            execvp(challenge_path,args);
            FATAL("%s", strerror(errno));
    }

    /* parent */
    waitpid(pid, 0, 0); // sync with execvp
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);

    for (;;) {
        /* Enter next system call */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));
        if (waitpid(pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));

        /* Gather system call arguments */
        struct user_regs uregs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &uregs) == -1)
            FATAL("%s", strerror(errno));



        long syscall_ = (long)uregs.uregs[7];

        /* Print a representation of the system call */
        // if(syscall_!=-1)
        fprintf(stderr, "%ld(%ld, %ld, %ld, %ld, %ld, %ld)",
                syscall_,
                (long)uregs.uregs[0], (long)uregs.uregs[1], (long)uregs.uregs[2],
                (long)uregs.uregs[3], (long)uregs.uregs[4],  (long)uregs.uregs[5]);


        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));
        if (waitpid(pid, 0, 0) == -1)
            FATAL("%s", strerror(errno));

        /* Get system call result */
        if (ptrace(PTRACE_GETREGS, pid, 0, &uregs) == -1) {

            // fputs(" = ?\n", stderr);
            if (errno == ESRCH && !(syscall_ == SYS_exit || syscall_ == SYS_exit_group)) {
                //exit(uregs.uregs[0]); // system call was _exit(2) or similar
                FATAL("%s, %ld", strerror(errno),(long)uregs.uregs[0]);
            }
            if(!(syscall_ == SYS_exit || syscall_ == SYS_exit_group))
                FATAL("%s", strerror(errno));
        }

        /* Print system call result */
        fprintf(stderr, " = %ld\n", (long)uregs.uregs[0]);
        if(syscall_!=-1) {
            ptrace_count[syscall_]++;
        }

        if(syscall_ == -1){
            fprintf(stderr, "[ERROR] orig_rax is -1\n");
          //  exit(EXIT_FAILURE);
            return;
            break;
        }
        else if(syscall_ == SYS_exit || syscall_ == SYS_exit_group){
            if((long)uregs.uregs[0]==0) {
                break;
            }
            else{
                fprintf(stderr, "[INFO] challenge exit code is %ld\n",(long)uregs.uregs[0]);
                break;
            }
        }
    }

    char ptrace_line[20];

    for(long i=0;i<1000000;i++){
        if(ptrace_count[i]>0){
            printf("%ld\t%d\n", i, ptrace_count[i]);
            sprintf(ptrace_line,"%ld\t%d\n", i, ptrace_count[i]);
            strcat(ptrace_send,ptrace_line);
        }
    }
}
#endif // !DEFINE_RPI
#endif // !DEFINE_ON_ARM


int main(int argc, char **argv) {

    SSL_CTX* ctx;

    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (SSL_CTX_load_verify_locations(ctx, CACERT, NULL) != 1) {
        fprintf(stderr, "SSL_CTX_load_verify_locations ");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_certificate_file(ctx, CLIENTCERT, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "SSL_CTX_use_certificate_file ");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, CLIENTKEY, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "SSL_CTX_use_PrivateKey_file ");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_check_private_key(ctx) != 1) {
        perror("SSL_CTX_check_private_key failed");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);


    int sock_in, client;
    sock_in=client_receiver(port);



    while(1) {

        /*socket in*/
        struct sockaddr_in addr_client;
        unsigned int len = sizeof(sockaddr_in);

        SSL* ssl;

        printf("====== waiting for composite_number ======\n");

        if ( (client = accept(sock_in, ((sockaddr *) &addr_client), &len)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }

        ssl = SSL_new(ctx);
        if (ssl == NULL) {
            perror("SSL_new error ");
            continue;
        }

        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            continue;
        }
        else {
            SSL_read(ssl, composite_number, sizeof(composite_number[0]) * 1000);
            cout << composite_number << endl;
        }


        cupaverage=0;
        cputag=0;

        pthread_t thread_challenge;
        pthread_t thread_getcpu;
        pthread_t thread_getmem;
        pthread_t thread_execurtion_time;


        pthread_create(&thread_challenge, NULL, run_challenge, NULL);
        pthread_create(&thread_getcpu, NULL, getcpu, NULL);
        pthread_create(&thread_getmem, NULL, getmem, NULL);
        pthread_create(&thread_execurtion_time, NULL, get_execurtion_time, NULL);

        pthread_join(thread_challenge, NULL);
        pthread_join(thread_getcpu, NULL);
        pthread_join(thread_getmem, NULL);
        pthread_join(thread_execurtion_time, NULL);


        printf("pthread ends . \n");


        FILE *cpu_txt = fopen("output_cpu.txt", "r");
/*        double previous_cpu_average = 0, current_cpu;
        for (int i = 1;; i++) {
            if (fscanf(cpu_txt, "%lf", &current_cpu) == EOF) {
                break;
            }
            previous_cpu_average = (previous_cpu_average * (i - 1) + current_cpu) / ((double) i);
        }

        printf("average cpu usage (from output_cpu.txt): %f\n", previous_cpu_average);*/
        printf("average cpu usage (from cupaverage): %f\n", cupaverage);

        printf("execution_time: %ld\n", execution_time);


        char buffer_sendback1[41000]; //challenge result + measurement
        memset(buffer_sendback1,'\0',sizeof(buffer_sendback1[0])*41000);
        sprintf(buffer_sendback1, "%s\n*\nVMSIZE= %d\nRSS= %d\nCPU_usage= %f\nexecurtion_time(10e-6s)= %ld\n@\n", buffer,
                virtual_proc_mem, proc_mem, cupaverage, execution_time);




        /* ptrace */
        USING_PTRACE();

        /*send*/
        int ssl_write_result;

        char buffer_send_all[141000];
        memset(buffer_send_all,'\0',sizeof(buffer_send_all[0])*141000);
        strcat(buffer_send_all,buffer_sendback1);
        strcat(buffer_send_all,ptrace_send);

        ssl_write_result = SSL_write(ssl, buffer_send_all, 141000);

        printf("send back: %d\n", ssl_write_result);
    }

    close(client);
    close(sock_in);
    SSL_CTX_free(ctx);

    return 0;
}