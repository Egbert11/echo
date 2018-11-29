#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

struct packet {
    unsigned short len;
    char buf[BUFFER_SIZE];
};