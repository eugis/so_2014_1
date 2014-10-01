#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <signal.h>

void
handle_signal(int signal){
  printf("Client is close.\n");
  exit(0);
}

void
install_signal_handler(){
  struct sigaction sa;
  
  // // Print pid, so that we can send signals from other shells
  // printf("My pid is: %d\n", getpid());

  // memset (&sa, '\0', sizeof(sa));

  // Setup the sighub handler
  sa.sa_handler = &handle_signal;

  // Restart the system call, if at all possible
  sa.sa_flags = SA_RESTART;

  // Block every signal during the handler
  sigfillset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
      // fatal("Error: cannot handle SIGINT");
  }
}

ipc_t* 
ipc_open(char *root){
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    ipc->s = socket(AF_UNIX, SOCK_STREAM, 0);
	if( ipc->s < 0){
		perror("socket");
		exit(1);
	}

	/*Create the address of the server.*/
	memset(&(ipc->name), 0, sizeof(struct sockaddr_un));

	(ipc->name).sun_family = AF_UNIX;
	strcpy((ipc->name).sun_path, root);
	ipc->len = sizeof((ipc->name).sun_family) + strlen((ipc->name).sun_path);

	return ipc;
}

void 
ipc_close(ipc_t *ipc){

}

void 
ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t length)
{
	message_t *msg = (message_t *)malloc(sizeof(message_t));

	msg->sender = ipc->s;
	strcpy(msg->content, message);
	msg->content_length = length;

	send(recipient, msg, sizeof(message_t) + msg->content_length, 0);
}

message_t* 
ipc_recv(ipc_t *ipc)
{
	message_t *msg = (message_t *)malloc(sizeof(message_t));	
	int nread;
	
	recv(ipc->s, msg, sizeof(msg) + 1024, 0);
	(msg->content)[msg->content_length] = '\0';
	printf("recibi de sender:%d el mensaje: %s\n",msg->sender, msg->content);
  return msg;
}