#include <string.h>
#include <stdio.h>

#include "../inc/database.h"


void movie_init(movie_t* movie, char* name, char* time) {
    strncpy(movie->name, name, MOVIE_NAME_LENGTH);
    strncpy(movie->time, time, MOVIE_TIME_LENGTH);

    movie->remaining_tickets = MOVIE_INITIAL_TICKETS;
    memset(movie->tickets, 0, sizeof(movie->tickets));
}
