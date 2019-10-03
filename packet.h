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
    uint32_t timestamp, CRC1, CRC2;
    void *payload;
} TRTP_packet;

TRTP_packet read_TRTP_packet(void *packet);
