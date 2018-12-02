#include<stdint.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include "protocol.h"

char *
pack_message(struct message *msg, uint16_t *length){
    *length = sizeof(uint32_t)*2+sizeof(uint16_t)+msg->length;
    char *buf = malloc(length+1);
    if(buf==NULL){
        perror("pack_message malloc fail");
        return NULL;
    }
    memset(buf, 0, sizeof(length+1));
    char *ptr = buf;
    *(uint32_t*)ptr = htonl(msg->sid);
    ptr += sizeof(uint32_t);
    *(uint32_t*)ptr = htonl(msg->cid);
    ptr += sizeof(uint32_t);
    *(uint16_t)ptr = htons(msg->length);
    ptr += sizeof(uint16_t);
    memcpy(ptr, msg->body, msg->length);
    return buf;
}

struct message *
unpack_message(char *buf){
    // 通过new产生的指针可以返回，不然属于栈内指针，函数执行完即失效
    struct message *msg = malloc(sizeof(struct message); 
    msg->sid = ntohl(*(uint32_t*)buf);
    buf += sizeof(uint32_t);
    msg->cid = ntohl(*(uint32_t*)buf);
    buf += sizeof(uint32_t);
    msg->length = ntohs(*(uint16_t*)buf);
    buf += sizeof(uint16_t);
    msg->body = malloc(msg->length+1);
    if(msg->body==NULL){
        perror("unpack_message malloc fail");
        return NULL;
    }
    memcpy(msg->body, buf, msg->length);
    msg-body[msg->length] = NULL;
    return msg;
}