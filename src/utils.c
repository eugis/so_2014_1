#include "../inc/utils.h"
#include "../inc/database.h"

ticket_t u_buy_ticket (database_t *database, int movie_i) {
    db_wlock(database);

    movie_t *movie = &(database->movies[movie_i]);

    if (movie == NULL) {
        printf("No existe %s\n", movie->name);
        return 0;
    }

    if (movie->remaining_tickets == 0) {
        printf("No hay mas tickets para %s\n", movie->name);
        return 0;
    }

    ticket_t id = 1505;

    movie->tickets[MOVIE_INITIAL_TICKETS - movie->remaining_tickets] = id;
    movie->remaining_tickets--;

    db_unlock(database);
    return id;
}

movie_t* u_get_ticket(database_t *database, ticket_t ticket) {
    db_rlock(database);

    int movie_i, ticket_i;
    movie_t* movie = NULL;

    for (movie_i = 0; movie_i < database->count; movie_i++) {
        for (ticket_i = 0; ticket_i < MOVIE_INITIAL_TICKETS; ticket_i++) {
            if (database->movies[movie_i].tickets[ticket_i] == ticket)
                movie = &(database->movies[movie_i]);
        }
    }
    
    db_unlock(database);
    return movie;
}

// int main() {
//     database_t database = db_open("db");

//     ticket_t ticket = u_buy_ticket(database, "Matrix");

//     printf("%d\n", ticket);

//     db_close(database);
// }