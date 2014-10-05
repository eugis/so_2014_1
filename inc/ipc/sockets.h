#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



typedef struct {
    uint16_t server_id;
    uint16_t id;

    int is_server;
    struct sockaddr_in address;
} ipc_t;

#endif