#include "../../inc/ipc/fifo.h"
#include "../../inc/utils.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>


static void touch(char *path) {
    FILE *file = fopen(path, "w");
    fclose(file);
    // check fopen() and fclose()
}


static int getlock(char *address) {
    char path[250];
    sprintf(path, "%s.lock", address);
    touch(path);

    key_t sysv_key = ftok(path, SYSV_KEY_FIFO);
    // check ftok() >= 0

    int semarray = semget(sysv_key, 1, IPC_CREAT | 0666);
    // check semget() >= 0

    return semarray;
}

static void rmlock(int semarray) {
    semctl(semarray, 0, IPC_RMID);
    // check semctl() != -1
}

static void acquire(int semarray) {
    struct sembuf sb = {
        .sem_num = 0,
        .sem_op  = -1,
        .sem_flg = 0
    };

    semop(semarray, &sb, 1);
    // check semop() != -1
}

static void release(int semarray) {
    struct sembuf sb = {
        .sem_num = 0,
        .sem_op  = +1,
        .sem_flg = 0
    };

    semop(semarray, &sb, 1);
    // check semop() != -1
}



ipc_t *ipc_create(char *root) {
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    ipc->root = strdup(root);
    mkdir(root, 0777);

    char path[250];
    sprintf(path, "%s/%d", root, getpid());
    mkfifo(path, 0666);
    // check file < 0 => fatal error!

    ipc->lock = getlock(path);

    semctl(ipc->lock, 0, SETVAL, 1);
    // check semctl()

    return ipc;
}


ipc_t *ipc_listen(char *dir) {
    ipc_t *ipc = ipc_create(dir);

    ipc->server_id = ipc->id = getpid();

    return ipc;
}


ipc_t *ipc_connect(char *file) {
    if (access(file, F_OK | R_OK | W_OK) != 0)
        return NULL;

    ipc_t *ipc = ipc_create(filepath(file));

    ipc->id = getpid();
    ipc->server_id = atoi(filename(file));

    return ipc;
}


void ipc_close(ipc_t* ipc) {
    char path[250];
    sprintf(path, "%s/%d", ipc->root, ipc->id);

    rmlock(ipc->lock);

    unlink(path);

    free(ipc->root);
    free(ipc);
}


void ipc_send(ipc_t* ipc, uint16_t recipient, void *content, uint16_t length) {
    char path[250];
    sprintf(path, "%s/%d", ipc->root, recipient);

    int lock = getlock(path);
    acquire(lock);

    int inbox = open(path, O_RDWR);
    // check inbox < 1 => Error connecting to server

    write(inbox, &(ipc->id), sizeof(ipc->id));
    write(inbox, &length, sizeof(length));
    write(inbox, content, length);

    release(lock);
    close(inbox);
}


message_t* ipc_recv(ipc_t* ipc) {
    char path[250];
    sprintf(path, "%s/%d", ipc->root, ipc->id);

    int inbox = open(path, O_RDWR);

    /* Read header */
    uint16_t sender;
    uint16_t content_length;

    read(inbox, &sender, sizeof(sender));
    read(inbox, &content_length, sizeof(content_length));

    /* Alloc and read content */
    size_t message_size = sizeof(message_t) + content_length;
    message_t* msg = (message_t*) malloc(message_size);
    read(inbox, msg->content, content_length);

    msg->sender = sender;
    msg->content_length = content_length;

    close(inbox);

    return msg;
}