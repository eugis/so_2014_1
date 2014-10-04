#include <stdio.h>

#include "../../inc/utils.h"
#include "../../inc/actions.h"


/* Forward-declared, defined at the end for clarity */
void handle_response(ipc_t *ipc);


int main(int argc, char** argv) {
    check(argc >= 3, "Usage: client <server address> <action> [params...]\n");

    char *server = argv[1];
    char *action = argv[2];

    int action_code = action_string_to_code(action);
    check(action_code, "Unknown action: %s\n", action);

    /* Valid arguments, valid command. Let's connect */

    ipc_t *ipc = ipc_connect(server);
    check(ipc != NULL, "Failed to connect to %s\n", server);


    switch (action_code) {
        case ACTION_MOVIE_LIST:
            check(argc == 3, "Usage: client <server address> list\n");

            req_movie_list(ipc);
        break;

        case ACTION_BUY_TICKET:
            check(argc == 4, "Usage: client <server address> buy <movie id>\n");

            int movie_id = atoi(argv[3]);

            req_buy_ticket(ipc, movie_id - 1);
        break;

        case ACTION_GET_TICKET:
            check(argc == 4, "Usage: client <server address> get <ticket id>\n");

            ticket_t ticket = atoi(argv[3]);

            req_get_ticket(ipc, ticket);
        break;
    }

    handle_response(ipc);
    ipc_close(ipc);
}

void handle_response(ipc_t *ipc) {
    message_t *msg = ipc_recv(ipc);
    res_any_t *res = (void*) &(msg->content);

    switch (res->type) {
        case ACTION_MOVIE_LIST:
            han_movie_list((res_movie_list_t*) res);
        break;

        case ACTION_BUY_TICKET:
            han_buy_ticket((res_buy_ticket_t*) res);
        break;

        case ACTION_GET_TICKET:
            han_get_ticket((res_get_ticket_t*) res);
        break;

        case ACTION_ERROR:
            han_error((res_error_t*) res);
        break;
    }

    free(msg);
}