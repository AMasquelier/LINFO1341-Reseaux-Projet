all: network.h packet.h main.c network.c packet.c
	gcc main.c network.c packet.c -o receiver
