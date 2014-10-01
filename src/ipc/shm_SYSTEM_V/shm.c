#include "../../inc/ipc/shm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>



void
fatal(char *s)
{
    perror(s);
    exit(1);
}

/*******************************************************/
/*********************IPC FUNCTION**********************/
/*******************************************************/
ipc_t* 
ipc_open(char *root){
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    ipc->id = getpid();
    ipc->shared_memory = get_memory(root);
    // ipc->root = strdup(root);

    init_mutex();
    install_signal_handler();

    return ipc;
}

void
ipc_close(ipc_t *ipc){
  close_mutex();
  free_memory();
  free(ipc);
}

void
install_signal_handler(){
  struct sigaction sa;
  
  // // Print pid, so that we can send signals from other shells
  // printf("My pid is: %d\n", getpid());

  // Setup the sighub handler
  sa.sa_handler = &handle_signal;

  // Restart the system call, if at all possible
  sa.sa_flags = SA_RESTART;

  // Block every signal during the handler
  sigfillset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
      fatal("Error: cannot handle SIGINT");
  }
}

void
handle_signal(int signal){
  close_mutex();
  free_memory();
  exit(0);
}

void 
ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t length)
{
  write(ipc->shared_memory, &(ipc->id),sizeof(ipc->id));
  write(ipc->shared_memory, &length,sizeof(length));
  write(ipc->shared_memory, message,length);
}

message_t* 
ipc_recv(ipc_t *ipc)
{
  uint16_t sender;
  uint16_t length;

  read(ipc->shared_memory, &sender, sizeof(sender));
  read(ipc->shared_memory, &length, sizeof(length));


  /* Alloc and read content: */
  message_t* msg = (message_t*) malloc(sizeof(message_t) + length);
  read(ipc->shared_memory, msg->content, length);

  msg->sender = sender;
  msg->content_length = length;
  return msg;
}


/*******************************************************/
/**********************SEMAPHORES***********************/
/*******************************************************/

static int semids;


void
init_mutex(void)
{
  if( (semid = semget(SHARED_KEY, 3, IPC_CREAT | 0666)) == -1 )
    fatal("semget failure");

  semctl(semid, 0, SETVAL, 1);

  semctl(semid, 1, SETVAL, 0);

  semctl(semid, 2, SETVAL, 0);

}

int
close_mutex(void){
	int exit_status = EXIT_SUCCESS;

  exit_status = semctl(semid, 0, IPC_RMID) != 0 );
	
	return exit_status;
}

 
void
dec(int numsem)
{
   struct sembuf sb;
  
  sb.sem_num = numsem - 1;
  sb.sem_op = -1;
  sb.sem_flg = SEM_UNDO;

  if( semop(semid,&sb,1) == -1 ) 
    fatal("semop failure");

}

void
inc(int numsem)
{
  struct sembuf sb;
  
  sb.sem_num = numsem - 1;
  sb.sem_op = 1;
  sb.sem_flg = SEM_UNDO;
  
  if( semop(semid,&sb,1) == -1 ) 
    fatal("semop failure");
}
/*******************************************************/
/********************SHARED MEMORY**********************/
/*******************************************************/

int
get_memory(char* root)
{
    int id, ft;
    key_t key;
    int data;

    if ((key = ftok("mem", 'r+')) == -1) {
        perror("ftok");
        exit(1);
    }

    /* connect to (and possibly create) the segment: */
    if ((id = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }


    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, (void *)0, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    return data;
}

void
free_memory(void){

    int exit_status;

    exit_status = shmdt("/mem");
    if(exit_status == -1) fatal("shm_unlink failure");
}
