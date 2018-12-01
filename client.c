#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "msgutils.c"

#define MYPORT 8887


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

    printf("connect success. conn:%d\n", socket_fd);

    struct packet recv_pack;
    struct packet send_pack;
    memset(&recv_pack, 0, sizeof(recv_pack));
    memset(&send_pack, 0, sizeof(send_pack));

    while(1){
        memset(&send_pack, 0, sizeof(send_pack));
        if(fgets(send_pack.buf, BUFFER_SIZE, stdin) == NULL){
            perror("fgets error");
            break;
        }
        if(strcmp(send_pack.buf, "\n") == 0)
            continue;
        unsigned short len = (unsigned short)strlen(send_pack.buf);
        send_pack.len = htons(len);
        int num = writen(socket_fd, &send_pack, len+2);
        // printf("writen buf:%s,len:%d, total_len:%d, write_num:%d\n",send_pack.buf,len,len+2,num);
        if(strcmp(send_pack.buf, "exit\n") == 0)
            break;
        memset(&recv_pack, 0, sizeof(recv_pack));
        int count = readn(socket_fd, &len, 2);
        if(count != 2){
            perror("read recv_pack error");
            exit(1);
        }
        len = ntohs(len);
        readn(socket_fd, recv_pack.buf, len);
        fputs(recv_pack.buf, stdout);
    }
    close(socket_fd);
    return 0;

}
