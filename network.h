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
} linked_buffer;

typedef struct Client
{
    struct in6_addr addr;
    linked_buffer *first;
    uint8_t buf_size;

    int file;
    uint8_t seqnum;
} Client;

int add_packet(Client *client, TRTP_packet *pkt);

int create_client(Client *c, struct sockaddr_in6 *serv_addr);

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window);

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window);

int create_socket(struct sockaddr_in6 *addr, int port);


#endif
