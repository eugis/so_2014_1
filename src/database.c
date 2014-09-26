#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../inc/database.h"


void movie_init(movie_t* movie, char* name, char* time) {
    strncpy(movie->name, name, MOVIE_NAME_LENGTH);
    strncpy(movie->time, time, MOVIE_TIME_LENGTH);

    movie->remaining_tickets = MOVIE_MAX_TICKETS;
    memset(movie->tickets, 0, sizeof(movie->tickets));
}


database_t *db_open(char *path) {
    FILE *db_file = fopen(path, "r+");

    struct stat st;
    fstat(fileno(db_file), &st);

    void *movies = mmap(
        NULL,                   /* Any memory address */
        st.st_size,             /* Length of mapping (entire file) */
        PROT_READ | PROT_WRITE, /* RW access */
        MAP_SHARED,             /* Multiprocess mapping */
        fileno(db_file),
        0                       /* Initial offset */
    );

    if (movies == MAP_FAILED) {
        printf("NOOOOOOOOOOOOOOOO %d\n", errno);
    }

    database_t *database = (database_t*) malloc(sizeof(*database));

    database->file   = db_file;
    database->st     = st;
    database->count  = st.st_size / sizeof(movie_t);
    database->movies = (movie_t*) movies;

    return database;
}


void db_close(database_t *database) {
    munmap(database->movies, database->st.st_size);
    fclose(database->file);
    free(database);
}


static void db_lock(database_t *database, int type) {
    struct flock fl;
        
    fl.l_type   = type;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fl.l_pid    = getpid();

    fcntl(fileno(database->file), F_SETLKW, &fl);
}

void db_rlock(database_t *database) {
    db_lock(database, F_RDLCK);
}

void db_wlock(database_t *database) {
    db_lock(database, F_WRLCK);  
}

void db_unlock(database_t *database) {
    struct flock fl;
        
    fl.l_type   = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fl.l_pid    = getpid();

    fcntl(fileno(database->file), F_SETLKW, &fl);
}



movie_t* db_find(database_t *database, char *name) {
    for (int i = 0; i < database->count; i++) {
        if (strcmp(database->movies[i].name, name) == 0)
            return &(database->movies[i]);
    }

    return NULL;
}


ticket_t db_buy_ticket (database_t *database, int movie_i) {
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

    /* As long as they are unique, ticket number generation doesn't matter */
    ticket_t ticket =
        (movie_i * MOVIE_MAX_TICKETS)
      + (MOVIE_MAX_TICKETS - movie->remaining_tickets) + 1
    ;

    movie->tickets[MOVIE_MAX_TICKETS - movie->remaining_tickets] = ticket;
    movie->remaining_tickets--;

    db_unlock(database);
    return ticket;
}


movie_t* db_get_ticket(database_t *database, ticket_t ticket) {
    db_rlock(database);

    int movie_i, ticket_i;
    movie_t* movie = NULL;

    for (movie_i = 0; movie_i < database->count; movie_i++) {
        for (ticket_i = 0; ticket_i < MOVIE_MAX_TICKETS; ticket_i++) {
            if (database->movies[movie_i].tickets[ticket_i] == ticket)
                movie = &(database->movies[movie_i]);
        }
    }
    
    db_unlock(database);
    return movie;
}
