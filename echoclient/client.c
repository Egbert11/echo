#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<errno.h>
#include<../protocol/echoprotocol.h>

#ifdef __WIN32__
#include<winsock2.h>
#include<ws2tcpip.h>
#else
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#endif

#define MYPORT 8887
#define BUFFER_SIZE 1024


int main(int argc, char const *argv[])
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        perror("socket init error");
        exit(1);
    }
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(connect(socket_fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1){
        perror("bind error");
        exit(1);
    }



    fprintf(stdout, "connect success. conn:%d\n", socket_fd);
    fflush(stdout);

    char send_buf[BUFFER_SIZE];
    char recv_buf[BUFFER_SIZE];

    char *res;
    while(1){
        memset(send_buf, 0, sizeof(send_buf));
        if(fgets(send_buf, BUFFER_SIZE, stdin) == NULL){
            perror("fgets error");
            break;
        }
        if(strcmp(send_buf, "\n") == 0)
            continue;
        send(socket_fd, send_buf, strlen(send_buf), 0);
        if(strcmp(send_buf, "exit\n") == 0)
            break;

        memset(recv_buf, 0, sizeof(recv_buf));
        int len = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
        fputs(recv_buf, stdout);
    }
    close(socket_fd);
    return 0;
}
