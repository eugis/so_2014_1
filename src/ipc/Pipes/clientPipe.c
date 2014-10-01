/*
 *  parte_july_client.c
 *  
 *
 *  Created by Julieta Sal-lari on 07/09/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */


#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdint.h>

#define MOVIE_MAX_TICKETS 100
#define MOVIE_NAME_LENGTH 100
#define MOVIE_TIME_LENGTH 100

typedef uint32_t ticket_t;

typedef struct movie {
    char name[MOVIE_NAME_LENGTH];
    char time[MOVIE_TIME_LENGTH];

    uint16_t remaining_tickets;
    ticket_t tickets[MOVIE_MAX_TICKETS];
} movie_t;

typedef struct {
    uint16_t id;
    char* root; /* inbox root directory */
} ipc_t;


typedef struct {
    uint16_t sender;
    uint16_t content_length;
    char content[];
} message_t;

typedef struct {
    uint8_t type;
} request_t;

typedef struct {
    uint8_t  type;
    uint16_t count;
	ticket_t ticket;
    movie_t movies[];
} response_t;

#define	NAME_LEN	30


static int serverFd = -1;
//static int clientFd = -1;
static char fifo_name[NAME_LEN];

void endSigInt(int signal){
	printf("wrong exit!\n");
	exit(1);
}

void initComm(){
	snprintf(fifo_name, NAME_LEN, "./tmp/client_fifo%d", getpid());
	
	if (access(fifo_name,0) == -1 && mknod(fifo_name, S_IFIFO|0666, 0) == -1) {
		printf("error creating client fifo: %d\n", errno);
		exit(1);
	}
	
	serverFd = open("./tmp/server_fifo", O_WRONLY);
    if(serverFd == -1){
        printf("NO %d .\n", errno);
        exit(1);
    }
	
	signal(SIGINT, endSigInt);
	
}



int endComm(){
	int closeServer, closeClient;
	closeServer = close(serverFd);
	unlink(fifo_name);
	if (closeServer == -1) {
		exit(1);
	}
	return 0;
	
}

int showMenu(){
    int choice = -1;
        printf("Please, select your choice:\n\
        1.Mostar peliculas\n\
        2.Comprar entrada\n\
        3.Salir\n\
    ");
    scanf("%d", &choice);
    getchar();

    return choice;
}
 
 int validateChoice(int choice){
    if(choice == 1 || choice == 2 || choice == 3){
        return 0;
    }
    return 1;
 }
 
void* req_movie_list() {
    request_t* req = (request_t*) malloc(sizeof(request_t));
    req->type = 1;//ACTION_MOVIE_LIST;
    return req;
}

void* req_buy_ticket() {
	request_t* req = (request_t*) malloc(sizeof(request_t));
    req->type = 2;    
	return req;
}

void* generateContent(int choice){
    if(choice == 1){
        return req_movie_list();
    }
	if (choice == 2) {
		return req_buy_ticket();
	}
    return NULL;
}

void ipc_send(ipc_t* ipc, uint16_t recipient, void *content, uint16_t length) {
        uint16_t sender;
        uint16_t content_length;

		write(serverFd, &(ipc->id), sizeof(ipc->id));
        write(serverFd, &content_length, sizeof(content_length));
        write(serverFd, content, content_length);
}

void han_movie_list(response_t *res) {
    for (int i = 0; i < res->count; i++)
        printf("[%d] %s\n", i, res->movies[i].name);
}

void han_buy_ticket(response_t *res) {
    if (res->ticket != 0)
        printf("%d\n", res->ticket);
    else
        printf("No se pudo comprar el ticket.");
}

void handleResponse(message_t *msg){
	response_t* res = (response_t*)(msg->content);
	switch (res->type) {
		case 1:
			han_movie_list(res);
			break;
		case 2:
			han_buy_ticket(res);
			break;
	}
}

int execRequest(ipc_t* ipc){
    uint16_t sender;
    uint16_t content_length;
    void* content;

	int clientFd = -1;
	int choice = 0;
	message_t* msg;
	
	while((choice = showMenu())!= 3){
	    if(validateChoice(choice) == 0){
	         //ipc send
            content = generateContent(choice);
            
	        ipc_send(ipc, ipc->id, &content, sizeof(content));
        
            //ipc rcv
            if ((clientFd = open(fifo_name, O_RDONLY)) == -1) {
		        printf("error opening client fifo \n");
		        exit(1);
	        }
            read(clientFd, &sender, sizeof(sender));
            read(clientFd, &content_length, sizeof(content_length));

            message_t* msg = (message_t*) malloc(sizeof(message_t) + content_length);
	
            read(clientFd, msg->content, content_length);
        
            msg->sender = sender;
            msg->content_length = content_length;
        
            if (close(clientFd) == -1) {
		        printf("error closing the client's fifo \n");
		        exit(1);
	        }
	        handleResponse(msg);
	        printf("cliente lee: %s\n", msg->content);
	    }else{
	        printf("Incorrect choice!\n");
	    }
	}
	return 0;
}


int main(){
	initComm();
	ipc_t *ipc;
	ipc->id = getpid();
	printf("WELCOME TO IPC-MOVIES\n");
	execRequest(ipc);

	endComm();
	 
}

