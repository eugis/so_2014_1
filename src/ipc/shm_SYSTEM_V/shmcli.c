#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "../inc/ipc.h"
#include "../inc/actions.h"
#include "../inc/utils.h"
#include "../inc/fixtures.h"

int
main()
{
    // char buf[SIZE-1];
    // int n;

    ipc_t* ipc;
    message_t* msg;

    ipc = ipc_open("mem");
    init_mutex();

    printf("CLIENT STARTS\n");

     while ( true ){

    dec(CLIENT_WRITE);

    req_get_ticket(ipc, server_pid, 1505);
        
        // n = read(0, buf, sizeof buf); //returns the number of bytes read is returned
        // buf[n]='\0';
        // printf("cliente escribe: %s, n:%d\n", buf, n);
        // ipc_send(ipc, getpid(), buf, n); //n is length
       
    inc(SERVER_READ);
    dec(CLIENT_READ);

          printf("pase ipc_recv\n");
          msg = ipc_recv(ipc);
          han_get_ticket((res_get_ticket_t*) &(msg->content));
          free(msg);
  
        // free(msg);
        // printf("pase CLIENT_READ\n");
        // msg = ipc_recv(ipc); //este ipc?
        // printf("termine CLIENT_READ\n");

    inc(CLIENT_WRITE);        
     }
}
