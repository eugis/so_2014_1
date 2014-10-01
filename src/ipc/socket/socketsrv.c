/* pollsrv.c */
#include "socket.h"
#include "fixtures.h"
#include "database.h"
#include "actions.h"

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


#define SOCKETNAME  "/tmp/mysocket"
#define MAXCLIENTS  10

int accept_client(int server);
int get_max_fd(int *clis, int n);
void new_client(int server, int *cfd, int *ncli);
void reorganize_clients_array(int *array, int *n);

int
get_max_fd(int *clis, int n)
{
	int i;
	int max;

	if (( clis == NULL) || (n<1))
		return 0;
		
	max = clis[0];
	for (i=0; i<n; i++)
		if (clis[i] > max)
			max = clis[i];

	return max;
}

int
accept_client(int server){
	socklen_t len_cli;
	struct sockaddr cli;
	int h;

	/*
	* La llamada a la funcion accept requiere que el parametro 
	* Longitud_Cliente contenga inicialmente el tamano de la
	* estructura Cliente que se le pase. A la vuelta de la
	* funcion, esta variable contiene la longitud de la informacion
	* util devuelta en Cliente
	*/
	len_cli = sizeof (cli);
	h = accept (server, &cli, &len_cli);
	printf("valor de h:%d\n", h);
	if (h == -1)
		return -1;

	/*
	* Se devuelve el descriptor en el que esta "enchufado" el cliente.
	*/
	return h;
}

void
new_client(int server, int *cfd, int *ncli){
	cfd[*ncli] = accept_client(server);
	(*ncli)++;

	/* Si se ha superado el maximo de clientes, se cierra la conexión,
	 * se deja todo como estaba y se vuelve. */
	if ((*ncli) >= MAXCLIENTS)
	{
		close (cfd[(*ncli) -1]);
		(*ncli)--;
		return;
	}
}

// void
// imprimir_array(int *array){
// 	int j;

// 	for(j = 0; j < 5; j++){
// 		printf("client[%d]: %d\n",j+1, array[j] );
// 	}
// }


void
reorganize_clients_array(int *array, int *n)
{
	int i,j;

	if ((array == NULL) || ((*n) == 0))
		return;

	j=0;
	for (i=0; i<(*n); i++)
	{
		if (array[i] != -1)
		{
			array[j] = array[i];
			j++;
		}
	}
	*n = j;
}

int
main(void)
{
	ipc_t *server;
	message_t *msg;
	ipc_t *aCli;

    char buf[1024];
	
	int nclients = 0;
	int clients[MAXCLIENTS];
	int i;
	int maxfd = 0;		/* descriptors up to maxfd-1 polled*/
	fd_set fds;			/* Set of file descriptors to poll*/

	database_t *database = db_open("db");

	/* Remove any previous socket.*/
	unlink(SOCKETNAME);

	/* Create the socket. */
	server = ipc_open(SOCKETNAME);
	aCli = (ipc_t*) malloc(sizeof(aCli));

	/*Bind the socket to the address.*/
	if (bind(server->s, (struct sockaddr *) &(server->name), server->len) < 0) {
		perror("bind");
		exit(1);
	}
 	
	/* Listen for connections. */
	if (listen(server->s, 5) < 0){
		perror( "listen");
		exit(1);
	}

	while(1){
		reorganize_clients_array(clients, &nclients);

		/* Set up polling using select. */
		FD_ZERO(&fds);
		FD_SET(server->s, &fds);

		for(i = 0; i < nclients; i++)
			FD_SET(clients[i], &fds);

		maxfd = get_max_fd(clients, nclients);

		if(maxfd < server->s)
			maxfd = server->s;

		select (maxfd + 1, &fds, NULL, NULL, NULL);

		for (i=0; i<nclients; i++)
		{
			if (FD_ISSET(clients[i], &fds))
			{
				sleep(2);
				aCli->s = clients[i];
				// msg = ipc_recv(aCli);	
			    msg = ipc_recv(aCli);
				if(msg->content_length > 0){
				    res_movie_list(aCli, database, msg->sender);
				    free(msg);
					printf ("Cliente %d envía %s\n", i+1, msg->content);
					ipc_send(server, clients[i], msg->content, msg->content_length);
				}
				else
				{
					printf ("Cliente %d ha cerrado la conexión\n", i+1);
					clients[i] = -1;
				}
			}
		}
		if (FD_ISSET(server->s, &fds)){
			new_client(server->s, clients, &nclients);
			printf("agrega cliente\n");
		}
	} 
}