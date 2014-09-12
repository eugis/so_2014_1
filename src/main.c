#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "../inc/ipc.h"
#include "../inc/actions.h"


int main() {
    int server = getpid();
    printf("S %d\n", server);

    if (fork() > 0) {
        database_t database = db_open("db");
        ipc_t* ipc = ipc_open("./tmp");

        message_t* msg = ipc_recv(ipc);

        res_movie_list(ipc, msg->sender, &database);

        // printf("S %p\n", msg);
        // printf("S %d, %d\n", msg->sender, msg->content_length);
        // printf("S %s\n", msg->content);

        free(msg);
        ipc_close(ipc);
        db_close(database);

    } else {
        int client = getpid();
        printf("C %d\n", client);

        ipc_t* ipc = ipc_open("./tmp");

        req_movie_list(ipc, server);

        message_t* msg = ipc_recv(ipc);
        res_movie_list_t* res = (res_movie_list_t*) &(msg->content);
        printf("C %s\n", res->movies[0].name);
        han_movie_list(res);
        free(msg);

        ipc_close(ipc);
    }
}
