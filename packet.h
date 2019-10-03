#include <stdlib.h>
#include <stdio.h>

typedef struct TRTP_packet
{
    uint32_t type : 2, tr : 1, window : 5, L : 1, length : 15, seqnum : 8;
} TRTP_packet;

TRTP_packet read_TRTP_packet(uint32_t header);
