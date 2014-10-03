#include <stdio.h>

#include "../../inc/utils.h"
#include "../../inc/actions.h"


int main(int argc, char** argv) {
    check(argc >= 3, "Usage: client <server address> <action> [params...]\n");

    char *server = argv[1];
    char *action = argv[2];

    int action_code = action_string_to_code(action);
    check(action_code, "Unknown action: %s\n", action);

    /* Valid arguments, valid command. Let's connect */

    ipc_t *ipc = ipc_connect(server);
    checkp(ipc, "Failed to connect to %s\n", server);

    message_t *msg;

    switch (action_code) {
        case ACTION_MOVIE_LIST:
            check(argc == 3, "Usage: client <server address> list");

            req_movie_list(ipc);

            msg = ipc_recv(ipc);
            han_movie_list((res_movie_list_t*) &(msg->content));
        break;

        case ACTION_BUY_TICKET:
            check(argc == 4, "Usage: client <server address> buy <movie id>");

            int movie_id = atoi(argv[3]);

            req_buy_ticket(ipc, movie_id - 1);

            msg = ipc_recv(ipc);
            han_buy_ticket((res_buy_ticket_t*) &(msg->content));
        break;

        case ACTION_GET_TICKET:
            check(argc == 4, "Usage: client <server address> get <ticket id>");

            ticket_t ticket = atoi(argv[3]);

            req_get_ticket(ipc, ticket);

            msg = ipc_recv(ipc);
            han_get_ticket((res_get_ticket_t*) &(msg->content));
        break;
    }

    free(msg);
    ipc_close(ipc);
}
