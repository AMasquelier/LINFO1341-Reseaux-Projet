#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

typedef struct TRTP_packet
{
    uint32_t    type : 2,
                tr : 1,
                window : 5,
                L : 1,
                length : 15,
                seqnum : 8;
    uint32_t timestamp, CRC1;
    void *payload;
    uint32_t CRC2;
} TRTP_packet;

void crc32(const void *data, size_t n_bytes, uint32_t* crc);

void display_byte_representation(void *data, long size);

void * make_ack(uint8_t seqnum, uint32_t timestamp);

TRTP_packet read_TRTP_packet(void *packet);
