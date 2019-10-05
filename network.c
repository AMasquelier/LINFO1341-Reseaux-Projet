#include "network.h"




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
