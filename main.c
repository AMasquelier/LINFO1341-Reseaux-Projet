#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "network.h"
#include "packet.h"



int main(int argc, char *argv[])
{
    int client = 0;
	int port = 1341;
	int opt;
	char *host = "::1";

    /*uint32_t test = 0xd18132f4;
    void * p = malloc(64);
    memcpy(p, &test, 32);
    memcpy(p+32, &test, 32);

    TRTP_packet pkt = read_TRTP_packet(p);


    printf("%d\n", pkt.length);
    printf("%d\n", pkt.L);
    printf("%zu\n", test);
    printf("%zu\n", pkt.timestamp);*/

    struct sockaddr_in6 serv_addr, client_addr;
    int err = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    printf("%d\n", err);


    return 0;
}
