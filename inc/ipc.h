#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>
#include "utils.h"


#if !defined(IPC) || IS_EMPTY(IPC)
    #error No IPC selected (compile with -DIPC)

    #undef IPC
    #define IPC NONE /* silence redundant errors */
#endif


typedef struct {
    uint16_t sender;
    uint16_t content_length;
    char content[];
} message_t;


/* Add all IPCs here */

#if IPC == FILESIG
    #include "ipc/filesig.h"
#elif IPC == SOCKETS
    #include "ipc/sockets.h"
#endif


#endif