#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>

#ifdef __WIN32__
#include<winsock2.h>
#include<ws2tcpip.h>
#else
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#endif

#define MYPORT 8887
#define QUEUE 20
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
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(socket_fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1){
        perror("bind error");
        exit(1);
    }

    if(listen(socket_fd, QUEUE) == -1){
        perror("listen error");
        exit(1);
    }

    char buf[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    int conn = accept(socket_fd, (struct sockaddr*)&client_addr, &length);
    if(conn == -1){
        perror("accept error");
        exit(1);
    }
    fprintf(stdout, "sockserver:%d, conn:%d\n", socket_fd, conn);
    fflush(stdout);

    while(1){
        memset(buf, 0, sizeof(buf));
        int len = recv(conn, buf, sizeof(buf), 0);
        if(strcmp(buf, "exit\n") == 0){
            break;
        }
        fputs(buf, stdout);
        send(conn, buf, len, 0);
    }
    close(conn);
    close(socket_fd);
    return 0;
}
