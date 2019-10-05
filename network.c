#include "network.h"


const char *real_address(const char *address, struct sockaddr_in6 *addr)
{
    char * ret =  (char *) malloc(64);
    struct addrinfo* result;
    struct addrinfo* res;
    int error;


    error = getaddrinfo(address, NULL, NULL, &result);
    if (error != 0) {
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }
        exit(EXIT_FAILURE);
    }

    for (res = result; res != NULL; res = res->ai_next) {
        char hostname[NI_MAXHOST];
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
        if (error != 0) {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname != '\0')
            printf("hostname: %s\n", hostname);
        strcpy(ret, hostname);
    }

    freeaddrinfo(result);
    return ret;
}

int create_socket(struct sockaddr_in6 *addr, int port)
{
    if (addr2 == NULL || port <= 0)
    {
        fprintf(stderr, "Bad input\n");
        return -1;
    }
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); // IPv6, UDP
    if (sock == -1) fprintf(stderr, "SOCKET CREATION ERROR\n");

    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = in6addr_any;


    int err;
    err = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (err < 0)
    {
        fprintf(stderr, "BINDING ERROR : %d\n", err);
        printf("%s\n", strerror(err));
        close(sock);
        return -1;
    }

    return 0;
}
