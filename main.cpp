#include <unistd.h>
#include <bits/stdc++.h>

#include<cerrno>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define btoa(x) ((x)?"true":"false")



using namespace std;


//char composite_number[]="77785013981551582343293798510443924965464555461232131231556156315";




char * get_execurtion_info(char *composite_number_char){
    char *buffer;
    buffer = (char *)malloc(9999*sizeof(char));


    char filename[1000];
    sprintf(filename,"../../DPI_challenge/cmake-build-debug/DPI_challenge %s",composite_number_char);

    char _usr_bin_time[]=R"(/usr/bin/time -f "\n*\nrealtime %e\nmemory_kb %M\ncpu %P"  2>&1)";

    char _usr_bin_time_command[1300];
    sprintf(_usr_bin_time_command, "%s %s",_usr_bin_time,filename);

    FILE *fp;
    fp= popen(_usr_bin_time_command,"r");

    char c;
    int i=0;
    while ( (c=fgetc(fp))!=EOF ){
        buffer[i]=c;
        i++;
    }

    pclose(fp);
    return buffer;
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

char* client_receiver(int port){
    char  *received_composite_number;
    received_composite_number= (char*) malloc(9999*sizeof(char));
    int  listenfd, connfd,             n;
    struct sockaddr_in  servaddr;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return nullptr;
    }

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



    struct sockaddr_in addr_client;
    unsigned int len = sizeof(sockaddr_in);


    printf("====== waiting for composite_number ======\n");

    if ( (connfd = accept(listenfd, ((sockaddr *) &addr_client), &len)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
        return nullptr;
    }

    n=recv(connfd, received_composite_number, 9999, 0);
    received_composite_number[n] = '\0';
    printf("receive composite_number[%d]: %s\n", n, received_composite_number);
    //for(int i=0;i<9999;i++)printf("%c",received_composite_number[i]);

    printf("accept ip address: %s\n", inet_ntoa(addr_client.sin_addr) );

    return received_composite_number;
}



int main(int argc, char **argv) {

    char *composite_number;
    composite_number=client_receiver(4321);

    char *ret_challenge;
    ret_challenge = get_execurtion_info(composite_number);

   // FILE *challenge_out= fopen("challenge_out.txt","w");

    for(auto i=0;i<9999;i++){
        if( ((char*)ret_challenge)[i]!='\0')
        {
            printf("%c",ret_challenge[i]);
//            fputc(ret_challenge[i],challenge_out);
        }
        else break;
    }
    printf("\n");



    bool send_data=client_send("192.168.33.1",4321,ret_challenge);
    printf("%s\n",btoa(send_data));

    return 0;
}