#ifndef __DATABASE_H__
#define __DATABASE_H__

#define MOVIE_INITIAL_TICKETS 100
#define MOVIE_NAME_LENGTH 100
#define MOVIE_TIME_LENGTH 100

typedef struct movie {
    char name[MOVIE_NAME_LENGTH];
    char time[MOVIE_TIME_LENGTH];

    int remaining_tickets;
    int tickets[MOVIE_INITIAL_TICKETS];
} movie_t;


void movie_init(movie_t* movie, char* name, char* time);

#endif
