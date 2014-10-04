#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include "../../inc/ipc.h"
#include "../../inc/actions.h"
#include "../../inc/utils.h"


int stop = 0; /* Setting this to true will stop the event loop */
database_t *database;
ipc_t *ipc;


void setup(char *address) {
    database = db_open("db");
    ipc = ipc_listen(address);
    debug("Listening on %s (PID %d)\n", address, getpid());
}

void teardown() {
    ipc_close(ipc);
    db_close(database);
    debug("Exiting\n");
    exit(0);
}

void exit_handler(int signum) {
    teardown();
}

void install_exit_handler() {
    struct sigaction handler = {
        .sa_flags   = 0,
        .sa_handler = exit_handler
    };

    sigaction(SIGINT, &handler, NULL);
    sigaction(SIGTERM, &handler, NULL);
}



int main(int argc, char **argv) {
    check(argc == 2, "Usage: server <listen address>\n");

    char *address = argv[1];

    setup(address);
    install_exit_handler();

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
                debug("ERROR bad req: %d is not an action type\n", req->type);
        }

        free(msg);
    }

    teardown();
}