#include "../../inc/ipc/shmsystemv.h"
#include "../../inc/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>


#include <unistd.h>
#include <signal.h>

static int id, server_id;
static key_t key; // used in get_memory and init_mutex
static message_t *data;

void
fatal(char *s)
{
    perror(s);
    exit(1);
}



/*******************************************************/
/**********************SEMAPHORES***********************/
/*******************************************************/

static int semid;

void
open_mutex(key_t key){
  if( (semid = semget(2014, 4, IPC_CREAT | 0666)) == -1 )
    fatal("semget failure");
}

void
init_mutex()
{
  semctl(semid, 0, SETVAL, 1);
  semctl(semid, 1, SETVAL, 0);
  semctl(semid, 2, SETVAL, 0);
  semctl(semid, 3, SETVAL, 0);
}

int
close_mutex(void){
  int exit_status = EXIT_SUCCESS;

  exit_status = (semctl(semid, 0, IPC_RMID) != 0);
  return exit_status;
}

// BORRAR ANTES DE ENTREGAR
void
values(){
  int valor = semctl (semid,0,GETVAL);
  printf("valor de sem1 es %d\n", valor);
  valor = semctl (semid,1,GETVAL);
  printf("valor de sem2 es %d\n", valor);
  valor = semctl (semid,2,GETVAL);
  printf("valor de sem3 es %d\n", valor);
  valor = semctl (semid,3,GETVAL);
  printf("valor de sem4 es %d\n", valor);
}

 
void
dec(int numsem)
{
  struct sembuf sb;
  
  sb.sem_num = numsem - 1;
  sb.sem_op = -1;
  sb.sem_flg = 0;//SEM_UNDO;

  if( semop(semid,&sb,1) == -1 ) 
    fatal("semop failure");
// values();
}

void
inc(int numsem)
{
  struct sembuf sb;

  sb.sem_num = numsem - 1;
  sb.sem_op = 1;
  sb.sem_flg = 0;//SEM_UNDO;
  
  if( semop(semid,&sb,1) == -1 ) 
    fatal("semop failure");

// values();
}


/*******************************************************/
/*********************IPC FUNCTION**********************/
/*******************************************************/
ipc_t*
ipc_listen(char *address){
  ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
  ipc->server_id = ipc->id = getpid();
  
  id = ipc->id;
  server_id = ipc->server_id;
  
  ipc->shared_memory = get_memory(address + 2, ipc);
  install_signal_handler();

  open_mutex(key);
  init_mutex(); // semaphores 0 0 0 0
  // values();
  return ipc;
}

ipc_t *ipc_connect(char *file) {
  ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
  ipc->id = getpid();
  ipc->server_id = atoi(filename(file));
  
  id = ipc->id;
  server_id = ipc->server_id;
  
  char *f = filepath(file);
  ipc->shared_memory = get_memory(f + 2, ipc);
  install_signal_handler();
  open_mutex(key);
  // values();
  return ipc;
}

void
ipc_close(ipc_t *ipc){
  if(ipc->id == ipc->server_id){ // is server?
    close_mutex();
    free_memory();
  }
  free(ipc);
}

void
install_signal_handler(){
  struct sigaction sa;

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
  if(id == server_id){
    close_mutex();
    free_memory();
  }
  exit(0);
}

void 
ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t length)
{
  if(ipc->id == ipc->server_id) dec(SERVER_WRITE);
  if(ipc->id != ipc->server_id) dec(CLIENT_WRITE);

  memcpy(&(ipc->shared_memory)->sender, &ipc->id, sizeof(ipc->id));
  memcpy(&(ipc->shared_memory)->content_length, &length, sizeof(length));
  memcpy((ipc->shared_memory)->content, message, length);      

  if(ipc->id == ipc->server_id) inc(CLIENT_READ);
  if(ipc->id != ipc->server_id) inc(SERVER_READ);
}

message_t* 
ipc_recv(ipc_t *ipc)
{
  uint16_t sender;
  uint16_t length;

  if(ipc->id == ipc->server_id) dec(SERVER_READ);
  if(ipc->id != ipc->server_id) dec(CLIENT_READ);

  memcpy(&sender, &(ipc->shared_memory)->sender, sizeof(sender));
  memcpy(&length, &(ipc->shared_memory)->content_length, sizeof(length)); 

  /* Alloc and read content: */
  size_t message_size = sizeof(message_t) + length;
  message_t* msg = (message_t*) malloc(message_size);
  memcpy(msg->content, (ipc->shared_memory)->content, length);

  msg->sender = sender;
  msg->content_length = length;

  if(ipc->id == ipc->server_id) inc(SERVER_WRITE);
  if(ipc->id != ipc->server_id) inc(CLIENT_WRITE);

  return msg;
}

/*******************************************************/
/********************SHARED MEMORY**********************/
/*******************************************************/

message_t*
get_memory(char* root, ipc_t *ipc)
{
  int shmid;
  int shmsz = sizeof(message_t);

  if ((shmid = shmget(2014, shmsz, 0666 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }

  /* attach to the segment to get a pointer to it: */
  data = shmat(shmid, NULL, 0);
  if (data == (message_t *)(-1)) {
    perror("shmat");
    exit(1);
  }

  return data;
}

void
free_memory()
{
  int exit_status;

  exit_status = shmdt(data);
  if(exit_status == -1) fatal("shm_unlink failure");
}
