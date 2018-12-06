#include<stdint.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include "protocol.h"
#include "msgutils.c"

// char *
// pack_message(struct message *msg, uint16_t *length){
//     *length = sizeof(uint32_t)*2+sizeof(uint16_t)+msg->length;
//     char *buf = malloc(length+1);
//     if(buf==NULL){
//         perror("pack_message malloc fail");
//         return NULL;
//     }
//     memset(buf, 0, sizeof(length+1));
//     char *ptr = buf;
//     *(uint32_t*)ptr = htonl(msg->sid);
//     ptr += sizeof(uint32_t);
//     *(uint32_t*)ptr = htonl(msg->cid);
//     ptr += sizeof(uint32_t);
//     *(uint16_t)ptr = htons(msg->length);
//     ptr += sizeof(uint16_t);
//     memcpy(ptr, msg->body, msg->length);
//     return buf;
// }

// struct message *
// unpack_message(char *buf){
//     // 通过new产生的指针可以返回，不然属于栈内指针，函数执行完即失效
//     struct message *msg = malloc(sizeof(struct message); 
//     msg->sid = ntohl(*(uint32_t*)buf);
//     buf += sizeof(uint32_t);
//     msg->cid = ntohl(*(uint32_t*)buf);
//     buf += sizeof(uint32_t);
//     msg->length = ntohs(*(uint16_t*)buf);
//     buf += sizeof(uint16_t);
//     msg->body = malloc(msg->length+1);
//     if(msg->body==NULL){
//         perror("unpack_message malloc fail");
//         return NULL;
//     }
//     memcpy(msg->body, buf, msg->length);
//     msg-body[msg->length] = NULL;
//     return msg;
// }

// header
char *pack_header(uint16_t sid, uint16_t cid){
    uint16_t size = sizeof(struct header);
    char *buf = malloc(size);
    memset(buf, 0, size);
    char *ptr = buf;
    sid = htons(sid);
    memcpy(ptr, &sid, 2);
    ptr += sizeof(uint16_t);
    cid = htons(cid);
    memcpy(ptr, &cid, 2);
    return buf;
}

struct header* unpack_header(char *head_str){
    struct header* head = malloc(sizeof(struct header));
    uint16_t sid;
    memcpy((char*)&sid, head_str, 2);
    head->sid = ntohs(sid);
    head_str += 2;
    uint16_t cid;
    memcpy((char*)&cid, head_str, 2);
    head->cid = ntohs(cid);
    return head;
}

// ChatProtocol
char *pack_chat_protocol(struct ChatProtocol* protocol, uint16_t *length){
    uint16_t size = CHAT_PROTOCOL_SIZE;
    char *buf = malloc(size);
    memset(buf, 0, size);
    uint16_t sender_len = strlen(protocol->sender)+1;
    uint16_t receiver_len = strlen(protocol->receiver)+1;
    uint16_t content_len = strlen(protocol->content)+1;
    char *ptr = buf;
    memcpy(ptr, protocol->sender, sender_len);
    ptr += (sender_len);
    memcpy(ptr, protocol->receiver, receiver_len);
    ptr += (receiver_len);
    memcpy(ptr, protocol->content, content_len);
    ptr += (content_len);
    *length = 0;
    *length += sender_len;
    *length += receiver_len;
    *length += content_len;
    return buf;
}

struct ChatProtocol* unpack_chat_protocol(char *protocol_str){
    struct ChatProtocol *protocol = malloc(sizeof(struct ChatProtocol));
    uint16_t sender_len = strlen(protocol_str)+1;
    protocol->sender = malloc(sender_len);
    memcpy(protocol->sender, protocol_str, sender_len);
    protocol_str += sender_len;
    uint16_t receiver_len = strlen(protocol_str)+1;
    protocol->receiver = malloc(receiver_len);
    memcpy(protocol->receiver, protocol_str, receiver_len);
    protocol_str += receiver_len;
    uint16_t content_len = strlen(protocol_str)+1;
    protocol->content = malloc(content_len);
    memcpy(protocol->content, protocol_str, content_len);
    return protocol;
}

// packet
// buf include len
char *pack_packet(char *head, char *body, uint16_t body_len){
    uint16_t header_len = sizeof(struct header);
    uint16_t total_len = header_len + body_len;
    char *buf = malloc(total_len + 2);
    char *ptr = buf;
    *(uint16_t*)ptr = htons(total_len);
    ptr += 2;
    memcpy(ptr, head, header_len);
    ptr += header_len;
    memcpy(ptr, body, body_len);
    return buf;
}

// buf not include len
void unpack_packet(char *buf, char **head, char **body, uint16_t total_len){
    // head
    char *ptr = buf;
    uint16_t header_len = sizeof(struct header);
    *head = malloc(header_len);
    memset(*head, 0, header_len);
    memcpy(*head, ptr, header_len);
    ptr += header_len;
    // body
    uint16_t body_len = total_len - header_len;
    *body = malloc(body_len);
    memset(*body, 0, body_len);
    memcpy(*body, ptr, body_len);
}
