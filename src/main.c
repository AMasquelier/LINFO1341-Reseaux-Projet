#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "network.h"
#include "packet.h"

char *create_name(const char *pattern, int n)
{
	char *name = (char*)malloc(sizeof(pattern) + 16);
	sprintf(name, pattern, n);
	return name;
}

int main(int argc, char *argv[])
{
	char *file_pattern = "file_%02d.dat";
	int nb_connections = 100;
	char *ip = "::";
	int port;


	// Reading args
	for (int i = 1; i < argc; i++)
	{
		printf("%s\n", argv[i]);
		if (strcmp("-o", argv[i]) == 0 && argc > i + 1)
		{
			file_pattern = argv[i+1];
			i++;
		}
		else if (strcmp("-m", argv[i]) == 0 && argc > i + 1)
		{
			nb_connections = atoi(argv[i+1]);
			if (nb_connections <= 0)
			{
				printf("WARNING : bad number of connections -> set to default (100)\n");
				nb_connections = 100;
			}
			i++;
		}
		else
		{
			ip = argv[i];
			if (argc > i + 1)
			{
				port = atoi(argv[i+1]);
				if (port <= 0)
				{
					printf("ERROR : bad port number\n");
					return -1;
				}
				i++;
			}
			else
			{
				printf("ERROR : invalid arguments\n");
				return -1;
			}
		}

	}


	fd_set rset;
    struct sockaddr_in6 serv_addr, client_addr;
    bzero(&client_addr, sizeof(client_addr));

    int sock = create_socket(&serv_addr, port);

	FD_ZERO(&rset);

    void *buf = malloc(528);

    //client clients[4];
	//printf("%s\n", create_name("Salut%02d.dat", n));

    int n = 0;
	linked_buffer *buffer = NULL;


    int keep = 1;

    Client client;
	create_client(&client, NULL);

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 150000;

    while (keep)
    {
        int n_rec = 0;

		FD_SET(sock, &rset);
		int nready = select(sock+1, &rset, NULL, NULL, &timeout);


		if (FD_ISSET(sock, &rset))
		{
			socklen_t clientsize = sizeof(client_addr);
			n_rec = recvfrom(sock, buf, 528, 0, (struct sockaddr *) &client_addr, &clientsize);


			TRTP_packet *p = read_TRTP_packet(buf);
			//print_packet(p);
			uint32_t CRC2 = crc32(0, p->payload, p->length);

			if (client.window == 0										||
				!is_in_window((client.seqnum+1)%256, WINDOW_SIZE, p->seqnum) ||
				p->type > 3 || p->type < 1 								||
				(p->tr != 0 && p->type != 1) 							||
				p->length > 512											||
				(p->CRC1 != p->nCRC1) 									||
				(p->type == 1 && p->length != 0 && CRC2 != p->CRC2))
			{
				// Ignore
				destroy_packet(p);
			}
	        else if (p->type == 1 && p->tr == 1)
	        {
	            send_nack(sock, &client_addr, p->seqnum, p->timestamp, client.window);
				destroy_packet(p);
		    }
	        else
	        {
				buffer = add_packet(buffer, &client, p);
				//if (buffer != NULL) printf("Buffer size : %d\n", buffer->size);
	        }
			//printf("Window : %d\n", client.window);
		}
		else
		{
			// Vide le buffer
			buffer = process_packet(buffer);
			//if (buffer != NULL) printf("Buffer size : %d\n", buffer->size);
			if (client.send_ack == 1) send_ack(sock, &client_addr, client.seqnum, client.timestamp, client.window);
			if (client.closed == 1)
			{
				keep = 0;
			}
		}
    }
	flush_buffer(buffer); //Vide le buffer au cas où il ne serait pas vide (Pas supposé arriver)
    free(buf);
	close(sock);

    return 0;
}