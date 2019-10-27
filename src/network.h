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

typedef struct Client
{
    int file;
    uint8_t seqnum;
    uint8_t closed;
    uint8_t window;
    uint32_t timestamp;
    uint8_t send_ack;

    struct Client *next;
    struct in6_addr addr;
    int size;
} Client;

typedef struct linked_buffer
{
    TRTP_packet *pkt;
    struct linked_buffer *next;
    Client *client;
    int size;
} linked_buffer;

void display_ip(struct in6_addr *addr);

int compare_ip(struct in6_addr *addr1, struct in6_addr *addr2);

int real_address(const char *address, struct sockaddr_in6 *rval);

void flush_buffer(linked_buffer *buffer);

void flush_clients(Client *clients);

int is_in_window(uint8_t min, uint8_t window, uint8_t seqnum);

Client *search(Client *first, struct sockaddr_in6 *addr);

linked_buffer* add_packet(linked_buffer *first, Client *client, TRTP_packet *pkt);

Client *add_client(Client *c, struct sockaddr_in6 *serv_addr, const char *filename);

Client *remove_client(Client *first, Client *to_remove);

linked_buffer* process_packet(linked_buffer *first);

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window);

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window);

int create_socket(struct sockaddr_in6 *addr, int port);


#endif
