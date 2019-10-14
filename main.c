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
	int window = WINDOW_SIZE;
	linked_buffer *buffer = NULL;


    int keep = 1;

    Client client;
	create_client(&client, NULL);

	struct timeval timeout;
	timeout.tv_sec = 4;
	timeout.tv_usec = 150000;

    while (keep)
    {
        int n_rec = 0;

		FD_SET(sock, &rset);
		int nready = select(sock+1, &rset, NULL, NULL, &timeout);

		if (window > 0 && FD_ISSET(sock, &rset))
		{
			socklen_t clientsize = sizeof(client_addr);
			n_rec = recvfrom(sock, buf, 528, 0, (struct sockaddr *) &client_addr, &clientsize);

			TRTP_packet *p = read_TRTP_packet(buf);
	        printf("type : %d\n", p->type);
	        printf("tr : %d\n", p->tr);
	        printf("window : %d\n", p->window);
	        printf("seqnum : %d\n", p->seqnum);
	        printf("L : %d\n", p->L);
	        printf("length : %d\n", p->length);
			printf("timestamp : %d\n", p->timestamp);

			uint32_t CRC2 = crc32(0, p->payload, p->length);

	        if ((p->CRC1 != p->nCRC1) || (p->type == 1 && p->length != 0 && CRC2 != p->CRC2))
	        {
	            send_nack(sock, &client_addr, p->seqnum, p->timestamp, window);
	        }
	        else
	        {
	            send_ack(sock, &client_addr, p->seqnum, p->timestamp, window);
				if (p->length > 0)
				{
					linked_buffer *_buf = add_packet(buffer, &client, p);
					if (_buf != NULL)
					{
						buffer = _buf;
						window--;
					}
				}
	            //int nw = write(file, p->payload, p->length);
	            //printf("written %d bytes \n", nw);
	        }
	        if (p->type == 1 && p->length == 0 /* && ... */)
	        {
				close(client.file);
				free(p);
				client.closed = 1;
	        }

			printf("\nwindow : %d\n", window);
	        printf("\n_________________________________________________________________________________\n\n");
		}
		else
		{
			// Vide le buffer
			while(window < WINDOW_SIZE)
			{
				linked_buffer *_buf = process_packet(buffer);

				buffer = _buf;
				window++;
			}
			if (client.closed == 1) keep = 0;
		}



        //printf("%d\n", n);



        //printf("%s\n", (char *)p->payload);




        /*struct hostent *hostp;
        hostp = gethostbyaddr((const char *)&client_addr.sin6_addr, sizeof(client_addr.sin6_addr), AF_INET6);
        printf("Host name: %s\n", hostp->h_name);*/


    }
    free(buf);
	close(sock);
    //close(file);
    return 0;
}
