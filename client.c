#include<stdlib.h>
#include<stdint.h>
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
#include "protocol.c"

#define MYPORT 8887

char *get_message(char *content, uint16_t *total_len){
    // header
    uint16_t header_len = sizeof(struct header);
    char *head = pack_header(41001, 1);
    // body
    struct ChatProtocol *proto = malloc(sizeof(struct ChatProtocol));
    proto->sender = malloc(CHAT_PROTOCOL_SIZE);
    proto->receiver = malloc(CHAT_PROTOCOL_SIZE);
    proto->content = malloc(CHAT_PROTOCOL_SIZE);
    strcpy(proto->sender, "john");
    strcpy(proto->receiver, "lily");
    strcpy(proto->content, content);
    uint16_t body_len;
    char *body = pack_chat_protocol(proto, &body_len);
    // merge head and body
    *total_len = header_len + body_len;
    char *buf = pack_packet(head, body, body_len);
    return buf;
}


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

    struct packet *recv_pack;
    struct packet *send_pack;
    recv_pack = malloc(sizeof(struct packet));
    send_pack = malloc(sizeof(struct packet));
    memset(recv_pack, 0, sizeof(struct packet));
    memset(send_pack, 0, sizeof(struct packet));
    
    recv_pack->buf = malloc(BUFFER_SIZE);
    memset(recv_pack->buf, 0, BUFFER_SIZE);

    char *data = malloc(BUFFER_SIZE);

    while(1){
        memset(data, 0, BUFFER_SIZE);
        if(fgets(data, BUFFER_SIZE, stdin) == NULL){
            perror("fgets error");
            break;
        }
        if(strcmp(data, "\n") == 0)
            continue;
        memset(send_pack, 0, sizeof(struct packet));
        char *send_buf = get_message(data, &(send_pack->len));
        // print_str(send_buf, send_pack->len+2);
        int num = writen(socket_fd, send_buf, send_pack->len+2);
        // printf("len:%d, write_num:%d\n",send_pack->len,num);
        if(strcmp(data, "exit\n") == 0)
            break;
        uint16_t len;
        int count = readn(socket_fd, &len, 2);
        if(count != 2){
            perror("read recv_pack error");
            exit(1);
        }
        len = ntohs(len);
        memset(recv_pack->buf, 0, BUFFER_SIZE);
        readn(socket_fd, recv_pack->buf, len);
        // print_str(recv_pack->buf, len);
        fputs(recv_pack->buf, stdout);
    }
    close(socket_fd);
    return 0;

}


