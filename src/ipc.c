#ifndef IPC
    #error No IPC selected (compile with -DIPC)

#else
    #if IPC == FILESIG
        #include "ipc/filesig.c"
    #endif
#endif
