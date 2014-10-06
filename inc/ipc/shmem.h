#ifndef __SHMEM_H__
#define __SHMEM_H__

#include <stdint.h>

/* Semaphores: */
#define CLIENT_CAN_WRITE 0
#define SERVER_CAN_READ 1
#define CLIENT_CAN_READ 2

#define SHARED_MEMORY_SIZE (1024 * 2)

#define SYSV_KEY_SHM 1
#define SYSV_KEY_SEM 2


typedef struct {
    uint16_t id;
    uint16_t server_id;
    void *memory;
    int semaphores;
} ipc_t;


ipc_t* ipc_listen(char *address);
ipc_t* ipc_connect(char *address);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recv(ipc_t *ipc);

#endif
