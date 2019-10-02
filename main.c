#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */
#include <sys/socket.h>
#include <netinet/in.h>

int create_socket(struct sockaddr_in6 *dest_addr, int dst_port)
{
    if (dest_addr == NULL || dst_port <= 0)
    {
        fprintf(stderr, "Bad input\n");
        return -1;
    }
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); // IPv6, UDP

    if (sock == 0) fprintf(stderr, "SOCKET CREATION ERROR\n");

    if (connect(sock, (struct sockaddr*)dest_addr, sizeof(dest_addr)) < 0)
    {
        fprintf(stderr, "CONNECTING ERROR\n");
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
	const char *err = real_address(host, &addr);
	if (err) {
		fprintf(stderr, "Could not resolve hostname %s: %s\n", host, err);
		return EXIT_FAILURE;
	}

    create_socket(&addr, port);

    return 0;
}
