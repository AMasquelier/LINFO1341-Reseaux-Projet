#include "packet.h"

// CRC32 implementation found on http://home.thep.lu.se/~bjorn/crc/
uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

void crc32(const void *data, size_t n_bytes, uint32_t* crc) {
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
}

void display_byte(uint8_t byte)
{
    uint8_t bits[8];
    for (int j = 0; j < 8;  j++) printf("%d", ((byte >> (7-j)) & 1));
    printf(" ");
}

void display_byte_representation(void *data, long size)
{
    uint8_t byte;
    for (long i = 0; i < size; i++)
    {
        memcpy(&byte, data + i, 1);
        display_byte(byte);
    }
    printf("\n");
    for (long i = 0; i < size; i++)
    {
        memcpy(&byte, data + i, 1);
        printf("%02X ", byte);
    }
    printf("\n");
}

void *make_ack(uint8_t seqnum, uint32_t timestamp)
{
    void *ret = malloc(15);
    TRTP_packet pck;
    pck.type    = 2;
    pck.tr      = 0;
    pck.window  = 0;
    pck.L       = 0;
    pck.length  = 0;
    pck.seqnum  = (seqnum + 1) % 256;
    pck.timestamp = timestamp;
    crc32(&pck, sizeof(pck), &pck.CRC1);
    pck.payload = NULL;
    crc32(&pck, sizeof(pck), &pck.CRC2);
}

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
    memcpy(&pkt.CRC1, packet + 7 + pkt.L, 4);
    memcpy(&pkt.CRC2, packet + 11 + pkt.L + pkt.length, 4);
    // payload
    pkt.payload = malloc(pkt.length);
    memcpy(&pkt.payload, packet + 11 + pkt.L, pkt.length);

    return pkt;
}
