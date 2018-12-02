#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>

ssize_t readn(int fd, void *buf, size_t count){
    int left = count;
    char *ptr = (char*)buf;
    int n;
    while(left > 0){
        n = read(fd, ptr, left);
        if(n == -1){
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK){
                continue;
            }
            return -1;
        }
        else if(n == 0){
            printf("peer close!\n");
            return count - left;
        }
        else{
            left -= n;
            ptr += n;
        }
    }
    return count;
}

ssize_t writen(int fd, void *buf, size_t count){
    int left = count;
    char *ptr = (char*)buf;
    int n;
    while(left > 0){
        n = write(fd, ptr, left);
        if(n == -1){
            /*
                Write will return a negative number if nothing is written under two circumstances:
                1.A temporary error (e.g. EINTR, EAGAIN, and EWOULDBLOCK); the first of these can happen 
                    with any write, the second two (broadly) only on non-blocking I/O.
                2.A permanent error.
            */
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK){
                continue;
            }
            return -1;
        }
        else if(n == 0){
            continue;
        }
        else{
            left -= n;
            ptr += n;
        }
    }
    return count;
}


int make_socket_non_blocking(int fd){
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