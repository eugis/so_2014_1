#ifndef __SHMSYSTEMV_H__
#define __SHMSYSTEMV_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CLIENT_WRITE 1
#define SERVER_READ	 2
#define CLIENT_READ  3
#define SERVER_WRITE 4

#define SIZE 100

typedef enum { false, true } bool;

typedef struct {
    uint16_t server_id;
    uint16_t id;
    message_t *shared_memory;
} ipc_t;



void fatal(char *s);

void init_mutex();
void open_mutex(key_t key);

int close_mutex(void);
void enter(int numsem);
void leave(int numsem);


message_t* get_memory(char* root, ipc_t *ipc);
void free_memory();
void handle_signal(int signal);
void install_signal_handler();

void dec(int numsem);
void inc(int numsem);


ipc_t* ipc_listen(char *address);
ipc_t* ipc_connect(char *file);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recv(ipc_t *ipc);

#endif
