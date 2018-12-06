#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include "protocol.h"

void print_str(char *,uint16_t);
void int_serialize(uint16_t, uint16_t, char*);

void main(int argc, char *argv[]){
    size_t size = sizeof(struct header);
    printf("header size:%d\n", size);
    char *buf = malloc(10);
    memset(buf, 0, 10);
    // char *src = "hello";
    // strncpy(buf, src, strlen(src));
    // print_str(buf, 10);
    
    char *serial_buf;
    int_serialize(41001,1,serial_buf);



}

void print_str(char *str, uint16_t str_len){
    uint16_t i = 0;
    for(; i<str_len; i++){
        printf("%02x",str[i]);
    }
    printf(";\n");
}

void int_serialize(uint16_t sid, uint16_t cid, char *buf){
    buf = malloc(10);
    memset(buf, 0, 10);
    char *ptr = buf;
    // strncpy(ptr, (char*)&sid, 2);
    
    *(uint16_t*)ptr = htons(sid);
    ptr+=2;
    // strncpy(ptr, (char*)&cid, 2);
    
    *(uint16_t*)ptr = htons(cid);
    print_str(buf, 4);
}