#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#define MOVIE_MAX_TICKETS 100
#define MOVIE_NAME_LENGTH 100
#define MOVIE_TIME_LENGTH 100

typedef uint32_t ticket_t;

typedef struct movie {
    char name[MOVIE_NAME_LENGTH];
    char time[MOVIE_TIME_LENGTH];

    uint16_t remaining_tickets;
    ticket_t tickets[MOVIE_MAX_TICKETS];
} movie_t;


typedef struct database {
    FILE* file;
    struct stat st;

    uint16_t count;
    movie_t* movies;
} database_t;


void movie_init(movie_t* movie, char* name, char* time);


database_t *db_open(char *path);
void db_close(database_t *database);

void db_rlock(database_t *database);
void db_wlock(database_t *database);
void db_unlock(database_t *database);

movie_t* db_find(database_t *database, char* name);

ticket_t db_buy_ticket (database_t *database, int movie_i);
movie_t* db_get_ticket(database_t *database, ticket_t ticket);

#endif
