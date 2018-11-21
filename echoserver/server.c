#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<errno.h>
#include<fcntl.h>

#ifdef __WIN32__
#include<winsock2.h>
#include<ws2tcpip.h>
#else
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#endif

#define MYPORT 8887
#define QUEUE 20
#define BUFFER_SIZE 1024
#define MAXEVENTS 64

static int make_socket_non_blocking(int fd){
    int flags, s;
    flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1){
        perror("fcntl F_GETFL error");
        return -1;
    }
    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if(s == -1){
        perror("fcntl F_SETFL error");
        return -1;
    }
    return 0;
}


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
                    char buf[BUFFER_SIZE];
                    memset(buf, 0, sizeof(buf));
                    ssize_t count = read(fd, buf, BUFFER_SIZE);
                    if(count == -1){
                        if(errno == EAGAIN){
                            perror("read EAGIN");
                        }
                        else{
                            perror("read other error");
                        }
                    }
                    else if(count == 0){
                        // connection close
                        perror("connection close.");
                        close(fd);
                    }
                    else {
                        // 因为buf本身带有换行符，故这个printf不加\n
                        printf("fd:%d %s", fd, buf);
                        ssize_t s = write(fd, buf, count);
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
