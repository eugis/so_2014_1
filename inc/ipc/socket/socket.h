#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdint.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


typedef enum { false, true } bool;

typedef struct {
    uint16_t sender;
    uint16_t content_length;
    char content[];
} message_t;

typedef struct {
    uint16_t s;
    int len;
    struct sockaddr_un name;
} ipc_t;


void handle_signal(int signal);
void install_signal_handler();

ipc_t* ipc_open(char *root);
void ipc_close(ipc_t *ipc);
void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recv(ipc_t *ipc);

#endif