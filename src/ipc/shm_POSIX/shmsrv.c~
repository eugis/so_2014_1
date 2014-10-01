#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "../inc/ipc.h"
#include "../inc/actions.h"
#include "../inc/utils.h"
#include "../inc/fixtures.h"

database_t *database;

int 
main()
{
  ipc_t* ipc;
  message_t* msg;
  
  printf("S %d\n", getpid());
  fixtures_read("./bin/db", "./fixtures.txt");
  database = db_open("./bin/db");
  ipc = ipc_open("mem");
  
  printf("INICIA SERVIDOR\n");
  test();
  inc(CLIENT_WRITE);
  while ( true ){
  dec(SERVER_READ);



  msg = ipc_recv(ipc);
  printf("PASE res_get_ticket\n");
  res_get_ticket(ipc, database, msg->sender, (req_get_ticket_t*) &(msg->content));
  free(msg);

   inc(CLIENT_READ);
  }
}

