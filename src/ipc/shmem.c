#include "../../inc/ipc/shmem.h"
#include "../../inc/utils.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>


static void *shalloc(char *address) {
    key_t sysv_key = ftok(address, SYSV_KEY_SHM);
    check(sysv_key != -1, "Cannot obtain shared memory\n");

    int id = shmget(sysv_key, SHARED_MEMORY_SIZE, 0666 | IPC_CREAT);
    check(id != -1, "Cannot obtain shared memory\n");

    void *memory = shmat(id, NULL, 0);
    check(memory != (void*) -1, "Cannot obtain shared memory\n");

    return memory;
}

static void shfree(void *memory) {
    shmdt(memory);
}


static int semcreate(char *address) {
    key_t sysv_key = ftok(address, SYSV_KEY_SEM);
    check(sysv_key != -1, "Cannot create semaphore\n");

    int semarray = semget(sysv_key, 3, IPC_CREAT | 0666);
    check(semarray != -1, "Cannot create semaphore\n");

    return semarray;
}

static void semdestroy(int semarray) {
    semctl(semarray, 0, IPC_RMID);
}

static void emit(int semarray, int semindex) {
    struct sembuf sb = {
        .sem_num = semindex,
        .sem_op  = 1,
        .sem_flg = 0
    };

    int ret = semop(semarray, &sb, 1);
    check(ret != -1, "Failed to emit() semaphore\n");
}

static void await(int semarray, int semindex) {
    struct sembuf sb = {
        .sem_num = semindex,
        .sem_op  = -1,
        .sem_flg = 0
    };

    int ret = semop(semarray, &sb, 1);
    check(ret != -1, "Failed to await() semaphore\n");
}


static ipc_t *ipc_open(char *address) {
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    ipc->memory = shalloc(address);
    ipc->semaphores = semcreate(address);

    return ipc;
}


ipc_t *ipc_listen(char *address) {
    ipc_t *ipc = ipc_open(address);

    ipc->server_id = ipc->id = getpid();

    return ipc;
}


ipc_t *ipc_connect(char *address) {
    ipc_t *ipc = ipc_open(address);

    ipc->id = getpid();
    ipc->server_id = 0;

    return ipc;
}


void ipc_close(ipc_t *ipc) {
    if (ipc->id == ipc->server_id) {
        semdestroy(ipc->semaphores);
        shfree(ipc->memory);
    }

    free(ipc);
}


void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t length) {
    int is_server = (ipc->id == ipc->server_id);

    if (! is_server)
        await(ipc->semaphores, CLIENT_CAN_WRITE);

    message_t *msg = (message_t*) ipc->memory;

    msg->sender = ipc->id;
    msg->content_length = length;
    memcpy(msg->content, message, length);

    emit(ipc->semaphores, is_server ? CLIENT_CAN_READ : SERVER_CAN_READ);
}


message_t* ipc_recv(ipc_t *ipc) {
    int is_server = (ipc->id == ipc->server_id);

    if (is_server) {
        emit(ipc->semaphores, CLIENT_CAN_WRITE);
        await(ipc->semaphores, SERVER_CAN_READ);
    } else
        await(ipc->semaphores, CLIENT_CAN_READ);


    /* Given the lock scheme, max 1 message is in memory at any time */
    message_t *orig  = (message_t*) ipc->memory;
    size_t total_len = sizeof(message_t) + orig->content_length;

    message_t *copy = (message_t*) malloc(total_len);
    memcpy(copy, orig, total_len);

    return copy;
}