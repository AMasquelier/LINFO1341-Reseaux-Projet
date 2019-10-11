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


    /*// Test format ACK
    void *pck = make_ack(0, 4);

    display_byte_representation(pck, 15);

    TRTP_packet p = read_TRTP_packet(pck);
    printf("type : %d\n", p.type);
    printf("tr : %d\n", p.tr);
    printf("window : %d\n", p.window);
    printf("seqnum : %d\n", p.seqnum);
    printf("timestamp : %d\n", p.timestamp);*/

    struct sockaddr_in6 serv_addr, client_addr;
    socklen_t clientsize = sizeof(client_addr);
    socklen_t servsize = sizeof(serv_addr);
    bzero(&client_addr, sizeof(client_addr));

    int sock = create_socket(&serv_addr, port);

    void * msg = malloc(528);


    int n;
    int keep = 1;

    int file = open("file.c", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    printf("file : %d\n", file);

    while (keep)
    {
        n = recvfrom(sock, msg, 528, 0, (struct sockaddr *) &client_addr, &clientsize);
        printf("%d\n", n);

        TRTP_packet *p = read_TRTP_packet(msg);
        printf("type : %d\n", p->type);
        printf("tr : %d\n", p->tr);
        printf("window : %d\n", p->window);
        printf("seqnum : %d\n", p->seqnum);
        printf("L : %d\n", p->L);
        printf("length : %d\n", p->length);

        //printf("%s\n", (char *)p->payload);


        uint32_t CRC2 = crc32(0, p->payload, p->length);

        if (p->type == 1 && p->length != 0 && CRC2 != p->CRC2)
        {
            printf("CRC2 different !  %d  :  %d \n", CRC2, p->CRC2);
            void *nack = make_nack(p->seqnum);
            sendto(sock, nack, 15, 0, (struct sockaddr *) &client_addr, clientsize);
            free(nack);
        }
        else if (p->CRC1 != p->nCRC1)
        {
            printf("CRC1 different !");
            void *nack = make_nack(p->seqnum);
            sendto(sock, msg, 15, 0, (struct sockaddr *) &client_addr, clientsize);
            free(nack);
        }
        else
        {
            void *ack = make_ack(p->seqnum, p->timestamp);
            sendto(sock, ack, 15, 0, (struct sockaddr *) &client_addr, clientsize);
            free(ack);
            int nw = write(file, p->payload, p->length);
            printf("written %d bytes \n", nw);
        }
        if (p->type == 1 && p->length == 0 /* && ... */)
        {
            keep = 0;
        }
        if (p != NULL) free(p);
        printf("\n_________________________________________________________________________________\n\n");
    }
    close(file);
    return 0;
}
