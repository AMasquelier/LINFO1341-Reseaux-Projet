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

#define WINDOW_SIZE 16

typedef struct Client
{
    struct in6_addr addr;

    int file;
    uint8_t seqnum;
    uint8_t closed;
    uint8_t window;
    uint32_t timestamp;
} Client;

typedef struct linked_buffer
{
    TRTP_packet *pkt;
    struct linked_buffer *next;
    Client *client;
    int size;
} linked_buffer;

void flush_buffer(linked_buffer *buffer);

int is_in_window(uint8_t min, uint8_t window, uint8_t seqnum);

linked_buffer* add_packet(linked_buffer *first, Client *client, TRTP_packet *pkt);

int create_client(Client *c, struct sockaddr_in6 *serv_addr);

linked_buffer* process_packet(linked_buffer *first);

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window);

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window);

int create_socket(struct sockaddr_in6 *addr, int port);


#endif
