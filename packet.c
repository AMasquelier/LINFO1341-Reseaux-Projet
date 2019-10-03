#include "packet.h"

TRTP_packet read_TRTP_packet(uint32_t header)
{
    TRTP_packet packet;

    uint8_t bits[32];
	for (int i = 31; i >= 0; i--) bits[31-i] = ((header >> i) & 1);

    // Type
	packet.type = bits[0] * 2 + bits[1];
    // Truncated ?
	packet.tr = bits[2];
    // Window
	for (int i = 0; i < 5; i++) packet.window = 2 * packet.window + bits[3 + i];
    // L
	packet.L = bits[8];
    // Length
	for (int i = 0; i < 7 + 8 * packet.L; i++) packet.length = 2 * packet.length + bits[9 + i];

	if (packet.type != 1 && packet.tr) printf("Ignored\n");

	if (packet.type == 1 && packet.length == 0 /* && ... */) printf("Transfer ended\n");

    return packet;
}
