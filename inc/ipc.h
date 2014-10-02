#ifndef __IPC_H__
#define __IPC_H__

#define CONCAT1(x) x ## 1
#define CONCAT2(x) CONCAT1(x)
#define IS_EMPTY(x) CONCAT2(x) == 1

#if !defined(IPC) || IS_EMPTY(IPC)
    #error No IPC selected (compile with -DIPC)

    #undef IPC
    #define IPC NONE /* silence redundant errors */
#endif


/* Add all IPCs here */

#if IPC == FILESIG
    #include "ipc/filesig.h"
#endif


#endif