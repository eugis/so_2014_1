#include <unistd.h>
#include <sys/types.h>

#include "../../inc/ipc.h"
#include "../../inc/actions.h"
#include "../../inc/utils.h"


int stop = 0; /* Setting this to true will stop the event loop */


int main(int argc, char **argv) {
    check(argc == 2, "Usage: server <listen address>\n");

    char *address = argv[1];

    printf("INFO  listening on %s (PID %d)\n", address, getpid());

    database_t *database = db_open("db");
    ipc_t *ipc = ipc_listen(address);
    message_t *msg;

    while (! stop) {
        msg = ipc_recv(ipc);

        req_any_t* req = (req_any_t*) &(msg->content);

        switch(req->type) {
            case ACTION_MOVIE_LIST:
                res_movie_list(ipc, database, msg->sender);
            break;

            case ACTION_BUY_TICKET:
                res_buy_ticket(ipc, database, msg->sender, (req_buy_ticket_t*) req);
            break;

            case ACTION_GET_TICKET:
                res_get_ticket(ipc, database, msg->sender, (req_get_ticket_t*) req);
            break;

            default:
                printf("ERROR bad req: %d is not an action type", req->type);
        }

        free(msg);
    }

    ipc_close(ipc);
    db_close(database);

    printf("INFO  exiting\n");
}