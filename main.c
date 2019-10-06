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

    void *pck = make_ack(0, 4);

    display_byte_representation(pck, 15);

    TRTP_packet p = read_TRTP_packet(pck);
    printf("type : %d\n", p.type);
    printf("tr : %d\n", p.tr);
    printf("window : %d\n", p.window);
    printf("seqnum : %d\n", p.seqnum);
    printf("timestamp : %d\n", p.timestamp);

    /*struct sockaddr_in6 serv_addr, client_addr;
    socklen_t clientsize = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));

    int sock = create_socket(&serv_addr, port);

    char msg[8192];


    int n = recvfrom(sock, (char*) msg, sizeof(msg), 0, (struct sockaddr *) &client_addr, &clientsize);
    printf("%d\n", n);
    printf("%s\n", msg);

    strncpy(msg, "Gneeeeh", sizeof(msg));
    sendto(sock, msg, sizeof(msg), 0, (struct sockaddr*) &client_addr, clientsize);*/

    return 0;
}
