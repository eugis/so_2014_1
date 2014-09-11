#ifndef __IPC_H__
#define __IPC_H__

#ifndef IPC
    #error No IPC selected (compile with -DIPC)

#else
    #if IPC == FILESIG
        #include "ipc/filesig.h"
    #endif
#endif

#endif