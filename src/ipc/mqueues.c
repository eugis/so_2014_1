#include "../../inc/ipc/mqueues.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

static ipc_t *ipc_open(char *address) {
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    key_t sysv_key = ftok(address, SYSV_KEY_QUEUE);
    // check ftok() != -1

    ipc->queue = msgget(sysv_key, 0666 | IPC_CREAT);
    // check msgget() != -1

    return ipc;
}


ipc_t *ipc_listen(char *address) {
    ipc_t *ipc = ipc_open(address);

    ipc->server_id = ipc->id = 1;

    return ipc;
}


ipc_t *ipc_connect(char *address) {
    ipc_t *ipc = ipc_open(address);

    ipc->id = getpid();
    ipc->server_id = 1;

    return ipc;
}


void ipc_close(ipc_t *ipc) {
    if (ipc->id == 1)
        msgctl(ipc->queue, IPC_RMID, NULL);

    free(ipc);
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t length) {
    ipc_queue_item_t *item = malloc(sizeof(item) + sizeof(message_t) + length);

    item->recipient_id = recipient;
    item->sender = ipc->id;
    item->content_length = length;
    memcpy(item->content, message, length);

    msgsnd(ipc->queue, item, sizeof(*item) - sizeof(long) + length, 0);
    // check(s != -1, "Failed to send message\n");

    free(item);
}

message_t *ipc_recv(ipc_t *ipc) {
    static char buf[MESSAGE_BUFFER_SIZE];

    int nbytes = msgrcv(ipc->queue, &buf, MESSAGE_BUFFER_SIZE, ipc->id, 0);
    // check nbytes != -1

    message_t *msg = malloc(nbytes);
    memcpy(msg, buf + sizeof(long), nbytes);

    return msg;
}