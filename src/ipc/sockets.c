#include "../../inc/ipc/sockets.h"
#include "../../inc/utils.h"

#include <arpa/inet.h>

ipc_t *ipc_listen(char *port_str) {
    int ret;
    int port = atoi(port_str);

    ipc_t *ipc = (ipc_t*) calloc(1, sizeof(ipc_t));

    ret = socket(AF_INET, SOCK_STREAM, 0);
    check(ret >= 0, "Cannot create socket\n");
    ipc->id = ret;


    ipc->address.sin_family = AF_INET;
    ipc->address.sin_addr.s_addr = INADDR_ANY;
    ipc->address.sin_port = htons(port);

    ret = bind(ipc->id, (struct sockaddr*) &(ipc->address), sizeof(ipc->address));
    check(ret != -1, "Cannot bind port %d\n", port);

    ret = listen(ipc->id, 5);
    check(ret != -1, "Cannot listen on port %d\n", port);

    ipc->server_id = ipc->id;
    ipc->is_server = 1;

    return ipc;
}


ipc_t *ipc_connect(char *address) {
    int ret;

    char *ip_host_str = strtok(address, ":");
    char *ip_port_str = strtok(NULL, ":");

    int port = atoi(ip_port_str);

    ipc_t *ipc = (ipc_t*) calloc(1, sizeof(ipc_t));

    ret = socket(AF_INET, SOCK_STREAM, 0);
    check(ret != -1, "Cannot create socket\n");
    ipc->id = ret;

    ipc->address.sin_family = AF_INET;
    ipc->address.sin_port = htons(port);

    ret = inet_pton(AF_INET, ip_host_str, &(ipc->address.sin_addr));
    check(ret == 1, "Cannot resolve address %s\n", address);

    ret = connect(ipc->id, (struct sockaddr*) &(ipc->address), sizeof(ipc->address));
    check(ret != -1, "Connection failed\n");

    ipc->server_id = ipc->id;

    return ipc;
}


message_t* ipc_recv(ipc_t *ipc) {
    int skt;

    if (ipc->is_server) {
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);

        skt = accept(ipc->id, (struct sockaddr*) &address, &addrlen);

    } else
        skt = ipc->id;

    uint16_t content_length;

    read(skt, &content_length, sizeof(content_length));

    message_t *msg = (message_t*) malloc(sizeof(message_t) + content_length);

    read(skt, msg->content, content_length);
    msg->sender = skt; /* Internally, we use the file descriptor */
    msg->content_length = content_length;

    return msg;
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size) {
    write(recipient, &size, sizeof(size));
    write(recipient, message, size);
}


void ipc_close(ipc_t *ipc) {
    close(ipc->id);
    free(ipc);
}