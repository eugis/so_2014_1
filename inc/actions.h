#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include <stdint.h>

#include "ipc.h"
#include "database.h"

/* All actions have a REQUEST function, a RESPONSE function, and a HANDLER
   function. Their flow is:

    client -> request()
    server -> response()
    client -> handler()

   Their respective serializable structures are `struct req_*` and
   `struct res_*`.
    
   All of these structures have a leading integer field named `type`, constant
   during the lifetime of the object and unique to each req/res/han class. This
   allows dynamic casting of passed messages.
*/

#define ACTION_MOVIE_LIST 1

typedef struct {
    uint8_t type;
} req_movie_list_t;

typedef struct {
    uint8_t  type;
    uint16_t count;
    movie_t movies[];
} res_movie_list_t;

void req_movie_list(ipc_t *ipc, uint16_t recipient);
void res_movie_list(ipc_t *ipc, uint16_t recipient, database_t *db);
void han_movie_list(res_movie_list_t *res);


#endif