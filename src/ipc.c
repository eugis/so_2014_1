#include "../inc/ipc.h"

#if defined(IPC_FILESIG)
    #include "ipc/filesig.c"

#elif defined(IPC_SOCKETS)
    #include "ipc/sockets.c"
#endif