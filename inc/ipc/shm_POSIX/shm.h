#ifndef __SHM_H__
#define __SHM_H__

#include <stdio.h>
#include <stdlib.h>
// #include <fcntl.h>
// #include <sys/stat.h>
// #include <sys/mman.h>
// #include <semaphore.h>
#include <stdint.h>

#define CLIENT_WRITE 1
#define SERVER_READ	 2
#define CLIENT_READ  3

#define SIZE 100

typedef enum { false, true } bool;

typedef struct {
    uint16_t sender;
    uint16_t content_length;
    char content[];
} message_t;

typedef struct {
    uint16_t id;
    int shared_memory;
    // char* semaforos;
} ipc_t;



void fatal(char *s);

void init_mutex(void);
int close_mutex(void);
void enter(int numsem);
void leave(int numsem);


int get_memory(char* root);
void free_memory(void);
void handle_signal(int signal);
void install_signal_handler();

void dec(int numsem);
void inc(int numsem);


ipc_t* ipc_open(char *root);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recv(ipc_t *ipc);

#endif
