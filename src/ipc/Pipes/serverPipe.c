/*
 *  parte_july.c
 *  
 *
 *  Created by Julieta Sal-lari on 06/09/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#include <string.h>
#include <sys/types.h>

#include <signal.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>

#include "database.h"
#include "fixtures.h"

#define	NAME_LEN	30

static char fifo_name[NAME_LEN];

static int serverFd = -1;
static int clientFd = -1;
static database_t *db;
int dummyFifo = -1;

#include <stdint.h>

typedef struct {
    uint8_t type;
	uint16_t movie_i;
} request_t;

typedef struct {
    uint8_t  type;
    uint16_t count;
	ticket_t ticket;
    movie_t movies[];
} response_t;

typedef struct {
    uint16_t id;
    char* root; /* inbox root directory */
} ipc_t;


typedef struct {
    uint16_t sender;
    uint16_t content_length;
    char content[];
} message_t;

void terminate(){
    printf("terminate on signal\n");
    exit(1);
}

void* res_movie_list() {
    size_t movie_list_size = sizeof(movie_t) * db->count;
    size_t total_res_size  = sizeof(response_t) + movie_list_size;

    response_t *res = (response_t*) malloc(total_res_size);

    res->type  = 1;//ACTION_MOVIE_LIST;
    res->count = db->count;
    memcpy(res->movies, db->movies, movie_list_size);
    
    return res;
}

void* res_buy_ticket(request_t *req) {
	ticket_t ticket = u_buy_ticket(db, req->movie_i);
	size_t total_res_size  = sizeof(response_t) + sizeof(ticket_t);
	
	response_t *res = (response_t*) malloc(total_res_size);
	
    res->type   = 2;
    res->ticket = ticket;
	
	memcpy(res->ticket, ticket, sizeof(ticket));
	
    return res;
}

void* handleContent(request_t* content){
    switch(content->type)
    {
        case 1: return res_movie_list(); break;
		case 2: return res_buy_ticket(content);break;
    }
    return NULL;
}

message_t* ipc_recv(ipc_t* ipc, int serverFd) {
    uint16_t sender;
    uint16_t content_length;
    
    read(serverFd, &sender, sizeof(sender));
    read(serverFd, &content_length, sizeof(content_length));
	
    message_t* msg = (message_t*) malloc(sizeof(message_t) + content_length);
    
    read(serverFd, msg->content, content_length);
    
    msg->sender = sender;
    msg->content_length = content_length;
     handleContent((request_t*)msg->content);

	    
    return msg;
}

void ipc_send(ipc_t* ipc, uint16_t recipient, void *content, uint16_t length) {
        int clientFd = -1;
        snprintf(fifo_name, NAME_LEN, "./tmp/client_fifo%d",/*msg->sender*/recipient);
		clientFd = open(fifo_name, O_WRONLY);
		if(clientFd == -1){
			printf("error opening client fifo: %d .\n", errno);
			exit(1);
		}
       
	    write(clientFd, &(ipc->id), sizeof(ipc->id));
        write(clientFd, &length, sizeof(length));
        write(clientFd, content, length);
	    printf("server escribe: %s\n", content);
	    
	    close(clientFd);
        
}


void handleRequest(){
}

int initServerComm(ipc_t* ipc){
	message_t* msg;
	int serverFd = -1;
	if (mknod("./tmp/server_fifo", S_IFIFO|0666, 0) == -1) {
		printf("error creating server fifo: %d\n", errno);
		exit(1);
	}
	/* 
	 Server process opens the PUBLIC fifo in write mode to make sure that 
	 the PUBLIC fifo is associated with atleast one WRITER process. As a 
	 result it never receives EOF on the PUBLIC fifo. The server process 
	 will block any empty PUBLIC fifo waiting for additional messages to 
	 be written. This technique saves us from having to close and reopen 
	 the public FIFO every time a client process finishes its activities. 
	 */
	serverFd = open("./tmp/server_fifo", O_RDONLY);
	dummyFifo = open("./tmp/server_fifo", O_WRONLY);
	
	if (serverFd == -1) {
		printf("error opening server fifo %d\n", errno);
		exit(1);
	}
	
	if (dummyFifo == -1) {
		printf("server problem found: %d\n", errno);
		exit(1);
	}
	
	while (1) {
	    signal(SIGPIPE, terminate);

        msg = ipc_recv(ipc, serverFd);
        
	    ipc_send(ipc, msg->sender, msg->content, msg->content_length);
	    
	}
	if(close(serverFd) == -1){
	    printf("error closing server!\n");
	}
}



ipc_t* ipc_open(char* root) {
    
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));

    ipc->id   = getpid();
    ipc->root = strdup(root);
	return ipc;
}



int main(){
	ipc_t* ipc;
	database_t *database;
	fixtures_read("./db", "./fixtures.txt");
	database = db_open("./bin/db");
	ipc = ipc_open("");
	initServerComm(ipc);
	//db_close(database);
}
