#ifndef __ECHO_PROTOCOL_H__
#define __ECHO_PROTOCOL_H__

#include<stdint.h>
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 65536
#endif
#ifndef HEADER_SIZE 
#define HEADER_SIZE 10
#endif
#ifndef CHAT_PROTOCOL_SIZE 
#define CHAT_PROTOCOL_SIZE 4096
#endif

struct packet {
    uint16_t len;
    char *buf;
};

struct header {
    uint16_t sid;
    uint16_t cid;
};

struct ChatProtocol {
    char *sender;
    char *receiver;
    char *content;
};

// header
char *pack_header(uint16_t sid, uint16_t cid);
struct header* unpack_header(char *head_str);

// ChatProtocol
char *pack_chat_protocol(struct ChatProtocol* protocol, uint16_t *length);
struct ChatProtocol* unpack_chat_protocol(char *protocol_str);

// packet
char *pack_packet(char *head, char *body, uint16_t body_len);
void unpack_packet(char *buf, char **head, char **body, uint16_t total_len);

#endif // __ECHO_PROTOCOL_H__
