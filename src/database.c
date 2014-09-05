#define _POSIX_C_SOURCE 1

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../inc/database.h"


void movie_init(movie_t* movie, char* name, char* time) {
    strncpy(movie->name, name, MOVIE_NAME_LENGTH);
    strncpy(movie->time, time, MOVIE_TIME_LENGTH);

    movie->remaining_tickets = MOVIE_INITIAL_TICKETS;
    memset(movie->tickets, 0, sizeof(movie->tickets));
}


database_t db_open(char* path) {
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

    return (database_t) {
        .file   = db_file,
        .st     = st,
        .count  = st.st_size / sizeof(movie_t),
        .movies = (movie_t*) movies,
    };
}


void db_close(database_t database) {
    munmap(database.movies, database.st.st_size);
    fclose(database.file);
}


movie_t* db_find(database_t database, char *name) {
    for (int i = 0; i < database.count; i++) {
        if (strcmp(database.movies[i].name, name) == 0)
            return &(database.movies[i]);
    }

    return NULL;
}