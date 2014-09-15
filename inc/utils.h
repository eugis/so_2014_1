#ifndef __UTILS_H__
#define __UTILS_H__

#include "database.h"

typedef int ticket_t;

ticket_t u_buy_ticket (database_t *database, char* movie_name);
movie_t* u_show_ticket(database_t *database, ticket_t ticket);

#endif
