#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

typedef struct TRTP_packet
{
    uint8_t     type    : 2,
                tr      : 1,
                window  : 5;
    uint16_t    L       : 1,
                length  : 15;
    uint8_t     seqnum;
    uint32_t    timestamp,
                CRC1, nCRC1;
    void *      payload;
    uint32_t    CRC2;
} TRTP_packet;

//void crc32(const void *data, size_t n_bytes, uint32_t* crc);
uint32_t crc32(uint32_t crc, char *buf, size_t len);

void display_byte_representation(void *data, long size);

void *make_ack(uint8_t seqnum, uint32_t timestamp);

void *make_nack(uint8_t seqnum, uint32_t timestamp);

TRTP_packet *read_TRTP_packet(void *packet);

#endif
