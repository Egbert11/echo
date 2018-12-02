#ifndef PROTOCOL_H
#define PROTOCOL_H
#include<stdint.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

struct packet {
    unsigned short len;
    char buf[BUFFER_SIZE];
};

struct message {
    uint32_t sid;
    uint32_t cid;
    uint16_t length;
    char *body;
};

#endif