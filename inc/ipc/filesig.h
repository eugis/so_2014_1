#ifndef __FILESIG_H__
#define __FILESIG_H__


#include <stdint.h>


typedef struct {
    uint16_t id;
    uint16_t server_id;
    char* root; /* inbox root directory */
} ipc_t;


#endif
