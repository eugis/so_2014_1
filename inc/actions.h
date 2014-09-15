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
#define ACTION_BUY_TICKET 2
#define ACTION_GET_TICKET 2

typedef struct {
    uint8_t type;
} req_movie_list_t;

typedef struct {
    uint8_t  type;
    uint16_t count;
    movie_t movies[];
} res_movie_list_t;

void req_movie_list(ipc_t *ipc, uint16_t recipient);
void res_movie_list(ipc_t *ipc, database_t *db, uint16_t sender);
void han_movie_list(res_movie_list_t *res);


typedef struct {
    uint8_t type;
    uint16_t movie_i;
} req_buy_ticket_t;

typedef struct {
    uint8_t type;
    ticket_t ticket;
} res_buy_ticket_t;

void req_buy_ticket(ipc_t *ipc, uint16_t recipient, uint16_t movie_i);
void res_buy_ticket(ipc_t *ipc, database_t *db, uint16_t sender, req_buy_ticket_t *req);
void han_buy_ticket(res_buy_ticket_t *res);


typedef struct {
    uint8_t type;
    ticket_t ticket;
} req_get_ticket_t;

typedef struct {
    uint8_t type;
    movie_t movie;
} res_get_ticket_t;

void req_get_ticket(ipc_t *ipc, uint16_t recipient, ticket_t ticket);
void res_get_ticket(ipc_t *ipc, database_t *db, uint16_t sender, req_get_ticket_t *req);
void han_get_ticket(res_get_ticket_t *res);


#endif