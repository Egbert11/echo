#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include "msgutils.c"

#define MYPORT 8887
#define QUEUE 20
#define MAXEVENTS 64


int main(int argc, char const *argv[])
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        perror("socket init error");
        exit(1);
    }
    int flag = 1, len = sizeof(int);
    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1){
        perror("setsockopt error");
        exit(1);
    }
    // 置为非阻塞
    make_socket_non_blocking(socket_fd);

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

    // epoll, size参数可以忽略，不再使用
    int epoll_fd = epoll_create(10);
    if(epoll_fd == -1){
        perror("epoll_create error");
        exit(1);
    }
    struct epoll_event ev;
    struct epoll_event *events;
    ev.data.fd = socket_fd;
    ev.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1){
        perror("epoll_ctl error");
        exit(1);
    }

    events = calloc(MAXEVENTS, sizeof(struct epoll_event));

    while(1){
        int i, n;
        // printf("epoll_waits\n");
        n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        if(n == -1){
            perror("epoll_wait error");
            exit(1);
        }
        // printf("epoll_wait done n=%d\n", n);
        for(i = 0; i < n; i++){
            if(socket_fd == events[i].data.fd){
                // 收到连接请求
                while(1){
                    struct sockaddr_in in_addr;
                    socklen_t in_len;
                    int infd;
                    infd = accept(socket_fd, (struct sockaddr *)&in_addr, &in_len);
                    if(infd == -1){
                        if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                            break;
                        }
                        else{
                            perror("accept error");
                            break;
                        }
                    }
                    printf("sockserver:%d, conn:%d\n", socket_fd, infd);
                    make_socket_non_blocking(infd); // 将连接置为非阻塞
                    struct epoll_event in;
                    in.data.fd = infd;
                    in.events = EPOLLIN | EPOLLRDHUP; // 对方close会同时发送EPOLLIN和EPOLLRDHUP，为了将这个与普通的输入区分开，将EPOLLRDHUP事件作为判断对方关闭连接的依据，关闭连接。
                    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, infd, &in) == -1){
                        perror("conn client epoll_ctl fail");
                        break;
                    }

                }
            }
            else{
                int fd = events[i].data.fd;
                printf("fd=%d, events:%s%s%s%s\n",fd,
                    (events[i].events & EPOLLIN)?"EPOLLIN ":"",
                    (events[i].events & EPOLLERR)?"EPOLLERR ":"",
                    (events[i].events & EPOLLHUP)?"EPOLLHUP ":"",
                    (events[i].events & EPOLLRDHUP)?"EPOLLRDHUP ":""
                );
                if(events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)){
                    printf("error occur, close fd:%d\n", fd);
                    /*移除fd，不过close的时候，如果fd的引用计数已经将为0，会自动移除。
                    这里是为了双重保险，所以加上EPOLL_CTL_DEL。可以参考man epoll的Q6。
                    */               
                    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL)==-1){
                        perror("epoll_ctl EPOLL_CTL_DEL error");
                    }
                    close(fd);
                }
                else if(events[i].events & EPOLLIN){
                    // char buf[BUFFER_SIZE];
                    // printf("check point\n");
                    struct packet pack;
                    memset(&pack, 0, sizeof(pack));
                    unsigned short len;
                    ssize_t count = readn(fd, &len, 2);
                    // printf("check point2 count:%d\n", count);
                    if(count != 2){
                        perror("read len error, connection close.");
                        close(fd);
                        continue;
                    }
                    else {
                        len = ntohs(len);
                        // printf("check point3 len:%d\n", len);
                        int read_num = readn(fd, pack.buf, len);
                        // buf本身包含\n，故不再使用换行符
                        printf("fd:%d, buf:%s", fd, pack.buf);
                        pack.len = htons(len);
                        ssize_t s = writen(fd, &pack, len+2);
                        // printf("fd:%d writen len:%d, buf:%s\n", fd, len+2, pack.buf);
                        if(s == -1){
                            perror("write error");
                        }
                    }
                }
                else{
                    printf("other events:%d\n", events[i].events);
                }
            }
        }
    }

    close(socket_fd);
    return 0;
}
