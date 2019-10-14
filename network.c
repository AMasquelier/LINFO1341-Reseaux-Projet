#include "network.h"

int compare_ip(struct in6_addr addr1, struct in6_addr addr)
{
    return 0;
}

linked_buffer* add_packet(linked_buffer *first, Client *client, TRTP_packet *pkt)
{
    if (pkt == NULL) return NULL;
    if (first == NULL)
    {
        linked_buffer *buf = (linked_buffer*) malloc(sizeof(linked_buffer));
        buf->pkt = pkt;
        buf->next = NULL;
        buf->client = client;

        return buf;
    }

    linked_buffer *act = first;

    while (act->next != NULL) act = act->next;

    linked_buffer *buf = (linked_buffer*) malloc(sizeof(linked_buffer));
    buf->pkt = pkt;
    buf->client = client;
    buf->next = NULL;
    act->next = buf;
    return first;

}

int create_client(Client *c, struct sockaddr_in6 *serv_addr)
{
    c->file = open("file.c", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    c->seqnum = 0;
    c->closed = 0;
    return 1;
}

linked_buffer* process_packet(linked_buffer *buffer)
{
    linked_buffer *buf = buffer;

    int nw = write(buffer->client->file, buffer->pkt->payload, buffer->pkt->length);
    printf("written %d bytes \n", nw);
    free(buffer->pkt->payload);
    free(buffer->pkt);
    buffer = buf->next;
    free(buf);
    return buffer;
}

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window)
{
    void *ack = make_ack(seqnum, timestamp, window);
    sendto(socket, ack, 11, 0, (struct sockaddr *) client, sizeof(*client));
    free(ack);
}

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window)
{
    void *nack = make_nack(seqnum, timestamp, window);
    sendto(socket, nack, 11, 0, (struct sockaddr *) client, sizeof(*client));
    free(nack);
}


int create_socket(struct sockaddr_in6 *addr, int port)
{
    if (addr == NULL || port <= 0)
    {
        fprintf(stderr, "Bad input\n");
        return -1;
    }
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); // IPv6, UDP
    if (sock == -1) fprintf(stderr, "SOCKET CREATION ERROR\n");

    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;


    int err;
    err = bind(sock, (struct sockaddr*)addr, sizeof(*addr));
    if (err < 0)
    {
        fprintf(stderr, "BINDING ERROR : %d\n", err);
        printf("%s\n", strerror(err));
        close(sock);
        return -1;
    }

    return sock;
}
