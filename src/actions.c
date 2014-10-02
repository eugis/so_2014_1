#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/actions.h"
#include "../inc/ipc.h"
#include "../inc/utils.h"


int action_string_to_code(char *action) {
    if (streq(action, "list"))
        return ACTION_MOVIE_LIST;
    else
    if (streq(action, "buy"))
        return ACTION_BUY_TICKET;
    else
    if (streq(action, "get"))
        return ACTION_GET_TICKET;
    else
        return 0;
}


/* List movies: */

void req_movie_list(ipc_t *ipc) {
    req_movie_list_t req = {
        .type = ACTION_MOVIE_LIST
    };

    ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_movie_list(ipc_t *ipc, database_t *db, uint16_t sender) {
    size_t movie_list_size = sizeof(movie_t) * db->count;
    size_t total_res_size  = sizeof(res_movie_list_t) + movie_list_size;

    res_movie_list_t *res = (res_movie_list_t*) malloc(total_res_size);

    res->type  = ACTION_MOVIE_LIST;
    res->count = db->count;
    memcpy(res->movies, db->movies, movie_list_size);

    ipc_send(ipc, sender, res, total_res_size);
    free(res);
}

void han_movie_list(res_movie_list_t *res) {
    for (int i = 0; i < res->count; i++)
        printf("[%d] %s\n", i, res->movies[i].name);
}


/* Buy ticket: */

void req_buy_ticket(ipc_t *ipc, uint16_t movie_i) {
    req_buy_ticket_t req = {
        .type    = ACTION_BUY_TICKET,
        .movie_i = movie_i
    };

    ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_buy_ticket(ipc_t *ipc, database_t *db, uint16_t sender, req_buy_ticket_t *req) {
    ticket_t ticket = db_buy_ticket(db, req->movie_i);

    res_buy_ticket_t res = {
        .type   = ACTION_BUY_TICKET,
        .ticket = ticket
    };

    ipc_send(ipc, sender, &res, sizeof(res));
}

void han_buy_ticket(res_buy_ticket_t *res) {
    if (res->ticket != 0)
        printf("Ticket: %d\n", res->ticket);
    else
        printf("No se pudo comprar el ticket.");
}


/* Get ticket: */

void req_get_ticket(ipc_t *ipc, ticket_t ticket) {
    req_get_ticket_t req = {
        .type   = ACTION_GET_TICKET,
        .ticket = ticket
    };

    ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_get_ticket(ipc_t *ipc, database_t *db, uint16_t sender, req_get_ticket_t *req) {
    movie_t *movie = db_get_ticket(db, req->ticket);

    res_get_ticket_t res = {
        .type  = ACTION_BUY_TICKET,
        .movie = *movie
    };

    ipc_send(ipc, sender, &res, sizeof(res));
}

void han_get_ticket(res_get_ticket_t *res) {
    printf("Ticket para: %s\n", res->movie.name);
}