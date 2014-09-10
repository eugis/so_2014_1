#include "../../inc/ipc/filesig.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>


static void signal_handler(int signum) {
    /* This handler will catch SIGUSR1 signals
       All we want is to be unblocked when the signal arrives,
       so this function does nothing.
    */
}


static void install_signal_handler() {
    struct sigaction handler = {
        .sa_flags   = 0,
        .sa_handler = signal_handler
    };

    sigaction(SIGUSR1, &handler, NULL);
}


static void flock(FILE *file, int type) {
    struct flock fl;
        
    fl.l_type   = type;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fl.l_pid    = getpid();

    fcntl(fileno(file), F_SETLKW, &fl);
}

static void frlock(FILE* file) {
    flock(file, F_RDLCK);
}

static void fwlock(FILE* file) {
    flock(file, F_WRLCK);  
}

static void funlock(FILE* file) {
    struct flock fl;
        
    fl.l_type   = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fl.l_pid    = getpid();

    fcntl(fileno(file), F_SETLKW, &fl);
}


static int fsize(FILE* file) {
    struct stat st;
    
    if (fstat(fileno(file), &st) != 0)
        return 0;

    return st.st_size;
}


ipc_t* ipc_open(char* root) {
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    ipc->id   = getpid();
    ipc->root = strdup(root);

    install_signal_handler();

    return ipc;
}


void ipc_close(ipc_t* ipc) {
    free(ipc->root);
    free(ipc);
}


int ipc_send(ipc_t* ipc, uint16_t recipient, void *content, uint16_t length) {
    char path[250];
    sprintf(path, "%s/%d", ipc->root, recipient);

    FILE *inbox = fopen(path, "a");
    fwlock(inbox);

    fwrite(&(ipc->id), sizeof(ipc->id), 1, inbox);
    fwrite(&length, sizeof(length), 1, inbox);
    fwrite(content, length, 1, inbox);

    funlock(inbox);
    fclose(inbox);

    /* Notify receipient: */
    kill(recipient, SIGUSR1);
}


message_t* ipc_recv(ipc_t* ipc) {
    char path[250];
    sprintf(path, "%s/%d", ipc->root, ipc->id);

    FILE *inbox = fopen(path, "w+");

    /* Check if the file has messages, wait if it doesn't: */
    while (fsize(inbox) == 0)
        pause();

    frlock(inbox);

    /* Read header: */
    uint16_t sender;
    uint16_t content_length;

    fread(&sender, sizeof(sender), 1, inbox);
    fread(&content_length, sizeof(content_length), 1, inbox);

    /* Alloc and read content: */
    message_t* msg = (message_t*) malloc(sizeof(message_t) + content_length);

    fread(msg->content, content_length, 1, inbox);
    msg->sender = sender;
    msg->content_length = content_length;

    funlock(inbox);
    fclose(inbox);

    return msg;
}


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