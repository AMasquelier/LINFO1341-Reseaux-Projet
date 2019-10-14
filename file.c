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
	uint32_t port = 1341;
	int opt;
	char *host = "::1";

    struct sockaddr_in6 serv_addr, client_addr;
    socklen_t clientsize = sizeof(client_addr);
    socklen_t servsize = sizeof(serv_addr);
    bzero(&client_addr, sizeof(client_addr));

    int sock = create_socket(&serv_addr, port);

    void * msg = malloc(528);

    //client clients[4];
	printf("       %s  ", client_addr.sin6_addr);

    int n = 0;
    int keep = 1;

    Client client;

    int file = open("file.c", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    printf("file : %d\n", file);
    uint8_t win_size = 4;
    while (keep)
    {
        int n_rec = recvfrom(sock, msg, 528, 0, (struct sockaddr *) &client_addr, &clientsize);
        //printf("%d\n", n);

        TRTP_packet *p = read_TRTP_packet(msg);
        printf("type : %d\n", p->type);
        printf("tr : %d\n", p->tr);
        printf("window : %d\n", p->window);
        printf("seqnum : %d\n", p->seqnum);
        printf("L : %d\n", p->L);
        printf("length : %d\n", p->length);
		printf("timestamp : %d\n", p->timestamp);
		printf("       %s  ", client_addr.sin6_addr);

        //printf("%s\n", (char *)p->payload);

        struct hostent *hostp;
        hostp = gethostbyaddr((const char *)&client_addr.sin6_addr, sizeof(client_addr.sin6_addr), AF_INET6);
        printf("Host name: %s\n", hostp->h_name);
        uint32_t CRC2 = crc32(0, p->payload, p->length);

        if ((p->CRC1 != p->nCRC1) || (p->type == 1 && p->length != 0 && CRC2 != p->CRC2))
        {
            send_nack(sock, &client_addr, p->seqnum, p->timestamp);
        }
        else
        {
            send_ack(sock, &client_addr, p->seqnum, p->timestamp);
            int nw = write(file, p->payload, p->length);
            //printf("written %d bytes \n", nw);
        }
        if (p->type == 1 && p->length == 0 /* && ... */)
        {
            keep = 1;
        }
        if (p != NULL) free(p);
        printf("\n_________________________________________________________________________________\n\n");
    }
    free(msg);
    close(file);
    return 0;
}
