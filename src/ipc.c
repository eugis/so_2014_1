#include "../inc/ipc.h"

#if IPC == FILESIG
    #include "ipc/filesig.c"

#elif IPC == SOCKETS
    #include "ipc/sockets.c"
#endif
