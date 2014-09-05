#include "../inc/utils.h"
#include "../inc/database.h"

ticket_t u_buy_ticket (database_t database, char* movie_name) {
    movie_t *movie = db_find(database, movie_name);

    if (movie == NULL) {
        printf("No existe %s\n", movie_name);
        return 0;
    }

    if (movie->remaining_tickets == 0) {
        printf("No hay mas tickets para %s\n", movie_name);
        return 0;
    }

    int id = 1505;

    movie->tickets[MOVIE_INITIAL_TICKETS - movie->remaining_tickets] = id;
    movie->remaining_tickets--;

    return id;
}

// movie_t* u_show_ticket(ticket_t ticket) {
// }

int main() {
    database_t database = db_open("db");

    ticket_t ticket = u_buy_ticket(database, "Matrix");

    printf("%d\n", ticket);

    db_close(database);
}