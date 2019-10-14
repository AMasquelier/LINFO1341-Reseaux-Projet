#include "packet.h"

// CRC32 implementation found on http://home.thep.lu.se/~bjorn/crc/
/*uint32_t crc32_for_byte(uint32_t r) {
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
}*/
uint32_t crc32(uint32_t crc, char *buf, size_t len)
{
	static uint32_t table[256];
	static int have_table = 0;
	uint32_t rem;
	uint8_t octet;
	int i, j;
	const char *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if (have_table == 0) {
		/* Calculate CRC table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320;
				} else
					rem >>= 1;
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for (p = buf; p < q; p++) {
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
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
    void *ret = malloc(11);
    bzero(ret, 11);

    ttw _ttw;
    _ttw.type    = 2;
    _ttw.tr      = 0;
    _ttw.window  = 1;
    memcpy(ret, &_ttw, 1);

    ls8 _ls;
    _ls.L        = 0;
    _ls.length   = 0;
    memcpy(ret + 1, &_ls, 1);

    uint8_t sn  = (seqnum + 1) % 256;
    memcpy(ret + 2, &sn, 1);

    timestamp = htonl(timestamp);
    memcpy(ret + 3, &timestamp, 4);

    uint32_t CRC1;
    CRC1 = crc32(0, ret, 7);
    CRC1 = htonl(CRC1);
    memcpy(ret + 7, &CRC1, 4);

    return ret;
}

void *make_nack(uint8_t seqnum, uint32_t timestamp)
{
    void *ret = malloc(11);
    bzero(ret, 11);

    ttw _ttw;
    _ttw.type    = 3;
    _ttw.tr      = 0;
    _ttw.window  = 0;
    memcpy(ret, &_ttw, 1);

    ls8 _ls;
    _ls.L        = 0;
    _ls.length   = 0;
    memcpy(ret + 1, &_ls, 1);

    uint8_t sn  = seqnum;
    memcpy(ret + 2, &sn, 1);

    memcpy(ret + 3, &timestamp, 4);

    uint32_t CRC1;
    CRC1 = crc32(0, ret, 7);
    CRC1 = htonl(CRC1);
    memcpy(ret + 7, &CRC1, 4);


    return ret;
}

TRTP_packet *read_TRTP_packet(void *packet)
{
    TRTP_packet *pkt = (TRTP_packet*)malloc(sizeof(TRTP_packet));
    uint32_t header;

    memcpy(&header, packet, 4);
    //display_byte_representation(&header, 4);
    header = htonl(header);
    uint8_t bits[32];
	for (int i = 31; i >= 0; i--) bits[31-i] = ((header >> i) & 1);
    //for (int i = 0; i < 32; i++) printf("%d", bits[i]); printf("\n");

    // Type
	pkt->type = bits[0] * 2 + bits[1];

    // Truncated ?
	pkt->tr = bits[2];
    // Window
	for (int i = 0; i < 5; i++) pkt->window = 2 * pkt->window + bits[3 + i];
    // L

	pkt->L = bits[8];
    // Length
	if (pkt->L)
	{
		uint16_t buf;
		memcpy(&buf, packet + 1, 2);
		buf = ntohs(buf);

		ls16 ls;
		memcpy(&ls, &buf, 2);
		pkt->length = ls.length;

	}
	else
	{
		ls8 ls;
		memcpy(&ls, packet + 1, 1);
		pkt->length = ls.length;
	}

    memcpy(&pkt->seqnum, packet + 2 + pkt->L, 1);
    if (pkt->L) pkt->seqnum = pkt->seqnum;

	if ((pkt->type != 1 && pkt->tr) || pkt->length > 512) printf("Ignored\n");

	if (pkt->type == 1 && pkt->length == 0 /* && ... */) printf("Transfer ended\n");

    // Timestamp
    memcpy(&pkt->timestamp, packet + 3 + pkt->L, 4);
    pkt->timestamp = ntohl(pkt->timestamp);
	uint32_t CRC1, CRC2;
    // CRC1
    pkt->nCRC1 = crc32(0, packet, 7 + pkt->L);
    memcpy(&pkt->CRC1, packet + 7 + pkt->L, 4);
    pkt->CRC1 = ntohl(pkt->CRC1);

    // CRC2
    memcpy(&pkt->CRC2, packet + 11 + pkt->L + pkt->length, 4);
    pkt->CRC2 = ntohl(pkt->CRC2);
	CRC2 = crc32(0, packet + 11 + pkt->L, pkt->length);

    // Payload
    pkt->payload = malloc(pkt->length);
    memcpy(pkt->payload, packet + 11 + pkt->L, pkt->length);


    return pkt;
}
