/* pollcli.c */
#include "socket.h"
#include "fixtures.h"
#include "database.h"
#include "actions.h"


#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <sys/un.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

#define SOCKETNAME  "/tmp/mysocket"


int
main(void)
{
	ipc_t *client;	
	message_t *msg;

    char buf[1024];

	int nread;	/* return from read() */
	int maxfd;	/* fd's 0 to maxfd-1 checked. */ 

	fd_set fds;	/* set of file descriptors to check. */

	install_signal_handler();

	client = ipc_open(SOCKETNAME);

	/*Connect to the server. If the connection wasn't possible, 
	  server closes this client */
	connect(client->s, (struct sockaddr *) &(client->name), client->len);
	
	maxfd = client->s + 1;

	while(1){
		/* Set up polling. */
		FD_ZERO(&fds);
		FD_SET(client->s,&fds);
		FD_SET(0,&fds);

		/* Wait for some input. */
		select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0,
				(struct timeval *) 0);

		/* If either device has some input,
		   read it and copy it to the other. */
		if( FD_ISSET(client->s, &fds))
		{
			// msg = ipc_recv(client);
			msg = ipc_recv(client);
    		han_movie_list((res_movie_list_t*) &(msg->content));
    		free(msg);
			if(nread < 1){
				close(client->s);
				exit(0);
			}
		}

		if( FD_ISSET(0, &fds))
		{
			// nread = read(0, buf, sizeof(buf));
			// /* If error or eof, terminate. */
			// if(nread < 1){
			// 	close(client->s);
			// 	exit(0);
			// }
			// ipc_send(client, client->s, buf, nread);
			req_movie_list(client, client->s);
		}
	} 

}