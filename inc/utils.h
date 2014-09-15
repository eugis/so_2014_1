#ifndef __UTILS_H__
#define __UTILS_H__

#include "database.h"

ticket_t u_buy_ticket (database_t *database, int movie_i);
movie_t* u_get_ticket(database_t *database, ticket_t ticket);

#endif
