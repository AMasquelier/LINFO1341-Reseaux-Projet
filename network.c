#include "network.h"

int compare_ip(struct in6_addr addr1, struct in6_addr addr)
{
    return 0;
}

int create_client(Client *c, struct sockaddr_in6 *serv_addr, uint32_t port)
{
    c->file = open("file.c", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    return 1;
}

int send_ack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp)
{
    void *ack = make_ack(seqnum, timestamp);
    sendto(socket, ack, 11, 0, (struct sockaddr *) client, sizeof(*client));
    free(ack);
}

int send_nack(int socket, struct sockaddr_in6 *client, uint8_t seqnum, uint32_t timestamp)
{
    void *nack = make_nack(seqnum, timestamp);
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
