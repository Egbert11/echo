#include "echoprotocol.h"

ssize_t readn(int fd, void *buf, size_t count){
    int left = count;
    char *ptr = (char*)buf;
    int n;
    while(left > 0){
        n = read(fd, ptr, left);
        if(n == -1){
            if(errno == EINTR){
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
            if(errno == EINTR){
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

int pack_message(struct packet *pack){
    memset(pack->buf, 0, sizeof(pack->buf));
    if(fgets(pack->buf, sizeof(pack->buf), stdin) != NULL){
        int n = strlen(pack->buf);
        pack->msgLen = htonl(n);
        return 0;
    }
    return -1;
}

int unpack_message(fd){
    struct packet pack;
    char readbuf[1026];
    memset(readbuf, 0, sizeof(readbuf));
    while(1){
        int n = read(fd, readbuf, 4);
        if (n == -1){
            if(errno == EAGAIN){

            }
        }
    }
}


