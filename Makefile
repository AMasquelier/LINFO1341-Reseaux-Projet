.PHONY: tests

all: src/network.h src/packet.h src/main.c src/network.c src/packet.c
	gcc src/main.c src/network.c src/packet.c -o receiver
build_tests:
	gcc tests/test.c -o tests/tests -lpthread &
	gcc tests/test2.c -o tests/tests2 -lpthread

tests:
	-./tests/tests
	-./tests/tests2
