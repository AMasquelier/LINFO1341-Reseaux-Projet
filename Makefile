all: src/network.h src/packet.h src/main.c src/network.c src/packet.c
	gcc src/main.c src/network.c src/packet.c -o receiver
