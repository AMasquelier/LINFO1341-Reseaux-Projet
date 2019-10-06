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


typedef struct ttw
{
    uint8_t window : 5, tr : 1, type : 2;
}ttw;

typedef struct ls8
{
    uint8_t length : 7, L : 1;
}ls8;

typedef struct ls16
{
    uint16_t length : 15, L : 1;
}ls16;


void *make_ack(uint8_t seqnum, uint32_t timestamp)
{
    void *ret = malloc(15);
    bzero(ret, 15);

    ttw _ttw;
    _ttw.type    = 2;
    _ttw.tr      = 0;
    _ttw.window  = 0;
    memcpy(ret, &_ttw, 1);

    ls8 _ls;
    _ls.L        = 0;
    _ls.length   = 0;
    memcpy(ret + 1, &_ls, 1);

    uint8_t sn  = (seqnum + 1) % 256;
    memcpy(ret + 2, &sn, 1);

    timestamp = htonl(timestamp);
    memcpy(ret + 3, &timestamp, 4);

    uint32_t CRC1, CRC2;
    crc32(ret, 15, &CRC1);
    CRC1 = htonl(CRC1);
    memcpy(ret + 7, &CRC1, 4);
    crc32(ret, 15, &CRC2);
    CRC2 = htonl(CRC2);
    memcpy(ret + 11, &CRC2, 4);

    return ret;
}

TRTP_packet read_TRTP_packet(void *packet)
{
    TRTP_packet pkt;
    uint32_t header;

    memcpy(&header, packet, 32);
    //display_byte_representation(&header, 4);
    header = htonl(header);
    uint8_t bits[32];
	for (int i = 31; i >= 0; i--) bits[31-i] = ((header >> i) & 1);
    for (int i = 0; i < 32; i++) printf("%d", bits[i]); printf("\n");

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

    memcpy(&pkt.seqnum, packet + 2 + pkt.L, 1);
    if (pkt.L) pkt.seqnum = ntohs(pkt.seqnum);

	if ((pkt.type != 1 && pkt.tr) || pkt.length > 512) printf("Ignored\n");

	if (pkt.type == 1 && pkt.length == 0 /* && ... */) printf("Transfer ended\n");

    // Timestamp
    memcpy(&pkt.timestamp, packet + 3 + pkt.L, 4);
    pkt.timestamp = htonl(pkt.timestamp);
    // CRC1
    memcpy(&pkt.CRC1, packet + 7 + pkt.L, 4);
    pkt.CRC1 = htonl(pkt.CRC1);
    memcpy(&pkt.CRC2, packet + 11 + pkt.L + pkt.length, 4);
    pkt.CRC2 = htonl(pkt.CRC2);
    // payload
    pkt.payload = malloc(pkt.length);
    memcpy(&pkt.payload, packet + 11 + pkt.L, pkt.length);

    return pkt;
}
