#include "../inc/actions.h"
#include "../inc/ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void req_movie_list(ipc_t *ipc, uint16_t recipient) {
    req_movie_list_t req = {
        .type = ACTION_MOVIE_LIST
    };

    ipc_send(ipc, recipient, &req, sizeof(req));
}

void res_movie_list(ipc_t *ipc, uint16_t recipient, database_t *db) {
    size_t movie_list_size = sizeof(movie_t) * db->count;
    size_t total_res_size  = sizeof(res_movie_list_t) + movie_list_size;

    res_movie_list_t *res = (res_movie_list_t*) malloc(total_res_size);

    res->type  = ACTION_MOVIE_LIST;
    res->count = db->count;
    memcpy(res->movies, db->movies, movie_list_size);

    ipc_send(ipc, recipient, res, total_res_size);
    free(res);
}

void han_movie_list(res_movie_list_t *res) {
    for (int i = 0; i < res->count; i++)
        printf("%s\n", res->movies[i].name);
}