#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "packet.h"


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

int create_socket(struct sockaddr_in6 *dest_addr, int dst_port)
{
    if (dest_addr == NULL || dst_port <= 0)
    {
        fprintf(stderr, "Bad input\n");
        return -1;
    }
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); // IPv6, UDP

    if (sock == 0) fprintf(stderr, "SOCKET CREATION ERROR\n");

    int err;
    if ((err = connect(sock, (struct sockaddr*)dest_addr, sizeof(dest_addr))) < 0)
    {
        fprintf(stderr, "CONNECTING ERROR : %d\n", err);
        return -1;
    }
    dest_addr->sin6_family = AF_INET6;
    dest_addr->sin6_addr = in6addr_any;
    dest_addr->sin6_port = htons(dst_port);

}

int main(int argc, char *argv[])
{
    int client = 0;
	int port = 12345;
	int opt;
	char *host = "::1";

    struct sockaddr_in6 addr;

    char *address = real_address(host, &addr);

    create_socket(&addr, port);

    return 0;
}
