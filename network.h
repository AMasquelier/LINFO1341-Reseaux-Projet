#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <inttypes.h>
#include "packet.h"

#define WINDOW_SIZE 4

typedef struct linked_buffer
{
    TRTP_packet *pkt;
    struct linked_buffer *next;
    uint8_t n;
} linked_buffer;

typedef struct Client
{
    struct in6_addr addr;

    linked_buffer buffers;
    uint8_t seqnum_min, available_space;
    int file;
} Client;

//int create_client(client *c, struct sockaddr *serv_addr, uint32_t port);

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp);

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp);

int create_socket(struct sockaddr_in6 *addr, int port);


#endif
