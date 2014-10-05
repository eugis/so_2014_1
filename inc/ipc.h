#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>
#include "utils.h"


typedef struct {
    uint16_t sender;
    uint16_t content_length;
    char content[];
} message_t;


/* Add all IPCs here */

#if defined(IPC_FILESIG)
    #include "ipc/filesig.h"

#elif defined(IPC_SOCKETS)
    #include "ipc/sockets.h"

#elif defined(IPC_SHMPOSIX)
    #include "ipc/shmposix.h"

#elif defined(IPC_SHMSYSTEMV)
    #include "ipc/shmsystemv.h"

#else
    #error No IPC selected (use make IPC=[FILESIG|SOCKETS])
#endif


ipc_t *ipc_listen(char *address);
ipc_t *ipc_connect(char *address);

void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recv(ipc_t *ipc);


#endif
