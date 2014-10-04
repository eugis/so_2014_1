#ifndef __FILESIG_H__
#define __FILESIG_H__


#include <stdint.h>


typedef struct {
    uint16_t id;
    uint16_t server_id;
    char* root; /* inbox root directory */
} ipc_t;


ipc_t *ipc_listen(char *address);
ipc_t *ipc_connect(char *address);

void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recv(ipc_t *ipc);

#endif
