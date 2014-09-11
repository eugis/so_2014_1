#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "../inc/ipc.h"


int main() {
    int parent = getpid();
    printf("Parent: %d\n", parent);

    if (fork() > 0) {
        ipc_t* ipc = ipc_open("./tmp");
        message_t* msg = ipc_recv(ipc);
        ipc_close(ipc);
        
        printf("%p\n", msg);
        printf("%d, %d\n", msg->sender, msg->content_length);
        printf("%s\n", msg->content);


    } else {
        sleep(1);

        int child = getpid();
        printf("Child: %d\n", child);

        ipc_t* ipc = ipc_open("./tmp");
        ipc_send(ipc, parent, (void*) "hola", 5);
        ipc_close(ipc);
    }
}
