#ifndef __FIFO_H__
#define __FIFO_H__


#include <stdint.h>

#define SYSV_KEY_FIFO 1

typedef struct {
    uint16_t id;
    uint16_t server_id;
    char *root;
    char *fifopath;
    char *lockpath;
    int lock;
} ipc_t;


#endif
