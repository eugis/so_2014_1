#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/fixtures.h"
#include "../inc/database.h"


static movie_t fixtures_read1(char *line) {
    movie_t movie;

    char *m_name = strtok(line, ",");
    char *m_time = strtok(NULL, ",");

    movie_init(&movie, m_name, m_time);

    return movie;
}


void fixtures_read(char *database_path, char *fixtures_path) {
    FILE *database = fopen(database_path, "w");
    FILE *fixtures = fopen(fixtures_path, "r");

    if (fixtures == NULL) {
        printf("NOOOOOOOOOOOOOOOO");
    }

    size_t size = 0;
    char  *line = NULL;

    while (getline(&line, &size, fixtures) != -1) {
        movie_t movie = fixtures_read1(line);
        fwrite(&movie, sizeof(movie), 1, database);
    }


    if (line)
        free(line);

    fclose(database);
    fclose(fixtures);
}