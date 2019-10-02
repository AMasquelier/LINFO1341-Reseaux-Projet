#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */
#include <sys/socket.h>
#include <netinet/in.h>

typename struct TRTP_packet
{
    int type = 0;
	bool tr = false;
	int window = 0;
	bool L = false;
	int Length = 0;
} TRTP_packet;

TRTP_packet read_TRTP_packet(uint32_t header)
{
    TRTP_packet packet;

    bool bits[32];

	for (int i = 31; i >= 0; i--) bits[31-i] = ((x >> i) & 1);


    // Type
	packet.type = bits[0] * 2 + bits[1];
    // Truncated ?
	packet.tr = bits[2];
    // Window
	for (int i = 0; i < 5; i++) packet.window = 2 * packet.window + bits[3 + i];
    // L
	packet.L = bits[8];
    // Length
	for (int i = 0; i < 7 + 8 * L; i++) packet.Length = 2 * packet.Length + bits[9 + i];
    
	if (packet.type != 1 && packet.tr) printf("Ignored"\n);

	if (packet.type == 1 && packet.Length == 0 /* && ... */) printf("Transfer ended"\n);

    return packet;
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
