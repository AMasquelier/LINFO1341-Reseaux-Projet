#include "packet.h"

TRTP_packet read_TRTP_packet(void *packet)
{
    TRTP_packet pkt;
    uint32_t header;

    memcpy(&header, packet, 32);


    uint8_t bits[32];
	for (int i = 31; i >= 0; i--) bits[31-i] = ((header >> i) & 1);

    // Type
	pkt.type = bits[0] * 2 + bits[1];
    // Truncated ?
	pkt.tr = bits[2];
    // Window
	for (int i = 0; i < 5; i++) pkt.window = 2 * pkt.window + bits[3 + i];
    // L
	pkt.L = bits[8];
    // Length
	for (int i = 0; i < 7 + 8 * pkt.L; i++) pkt.length = 2 * pkt.length + bits[9 + i];

	if ((pkt.type != 1 && pkt.tr) || pkt.length > 512) printf("Ignored\n");

	if (pkt.type == 1 && pkt.length == 0 /* && ... */) printf("Transfer ended\n");

    // Timestamp
    memcpy(&pkt.timestamp, packet + 3 + pkt.L, 4);
    // CRC1
    memcpy(&pkt.CRC1, packet + 7 + pkt.L 4);
    memcpy(&pkt.CRC2, packet + 11 + pkt.L + pkt.length, 4);
    // payload
    payload = malloc(pkt.length);
    memcpy(&pkt.payload, packet + 11 + pkt.L, pkt.length);

    return pkt;
}
