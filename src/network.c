#include "network.h"

int compare_ip(struct in6_addr addr1, struct in6_addr addr)
{
    return 0;
}

void flush_buffer(linked_buffer *buffer)
{
    if (buffer != NULL)
    {
        destroy_packet(buffer->pkt);
        linked_buffer *next = buffer->next;
        if (buffer != NULL) free(buffer);
        flush_buffer(next);
    }
}

linked_buffer* add_packet(linked_buffer *first, Client *client, TRTP_packet *pkt)
{
    if (pkt == NULL) return first;
    if (first == NULL)
    {
        linked_buffer *buf = (linked_buffer*) malloc(sizeof(linked_buffer));
        buf->pkt = pkt;
        buf->next = NULL;
        buf->client = client;
        buf->size = 1;
        client->window--;
        return buf;
    }
    if (pkt->seqnum == first->pkt->seqnum)
    {
        destroy_packet(pkt);
        return first;
    }
    first->next = add_packet(first->next, client, pkt);
    if(first->next != NULL) first->size = first->next->size + 1;

    return first;
}

int create_client(Client *c, struct sockaddr_in6 *serv_addr)
{
    c->file = open("file.c", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    c->seqnum = 255;
    c->closed = 0;
    c->window = WINDOW_SIZE;
    c->send_ack = 0;
    return 1;
}

linked_buffer* process_packet(linked_buffer *buffer)
{
    if (buffer == NULL) return NULL;
    //printf("1-Looking for %d  \n", buffer->client->seqnum);
    //printf("1-Looking %d  \n", buffer->pkt->seqnum);

    if ((buffer->client->seqnum+1)%256 == buffer->pkt->seqnum)
    {
        //printf("2-Looking %d  \n", buffer->client->seqnum);
        if (buffer->pkt->length > 0)
        {
            int nw = write(buffer->client->file, buffer->pkt->payload, buffer->pkt->length);
            //printf("written %d bytes \n", nw);
        }
        else
        {
            close(buffer->client->file);
            buffer->client->closed = 1;
        }
        buffer->client->seqnum = (buffer->client->seqnum+1)%256;
        buffer->client->send_ack = 1;
        buffer->client->timestamp = buffer->pkt->timestamp;
        destroy_packet(buffer->pkt);
        buffer->client->window++;

        linked_buffer *next = buffer->next;

        if (buffer != NULL) free(buffer);

        return next;
    }

    if (buffer->next != NULL)
    {
        buffer->next = process_packet(buffer->next);
        if (buffer->next != NULL) buffer->size = buffer->next->size + 1;
    }
    return buffer;
}

int is_in_window(uint8_t min, uint8_t window, uint8_t seqnum)
{
    //printf("Is in window : %d, %d, %d \n ", min, window, seqnum);
    return ((seqnum >= min && seqnum < min + window) ||
            (min + seqnum > 255 && seqnum >= min && seqnum < (min + window)%256));
}

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window)
{
    void *ack = make_ack(seqnum, timestamp, window);
    if (ack != NULL)
    {
        sendto(socket, ack, 11, 0, (struct sockaddr *) client, sizeof(*client));
        free(ack);
    }
}

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp, uint8_t window)
{
    void *nack = make_nack(seqnum, timestamp, window);
    if (nack != NULL)
    {
        sendto(socket, nack, 11, 0, (struct sockaddr *) client, sizeof(*client));
        free(nack);
    }
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
