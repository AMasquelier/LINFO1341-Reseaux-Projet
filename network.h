#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const char *real_address(const char *address, struct sockaddr_in6 *addr);

int create_socket(struct sockaddr_in6 *addr, int port);
