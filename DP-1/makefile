CC = gcc
CFLAGS = -I./inc -c -Wall
LDFLAGS = -lrt -lpthread

all: DP-1

DP-1: obj/dp1.o
	$(CC) -o bin/DP-1 obj/dp1.o $(LDFLAGS)
obj/dp1.o: src/dp1.cpp
	$(CC) -c $(CFLAGS) src/dp1.cpp -o obj/dp1.o

clean:
	rm -rf bin/DP-1 obj/*.o
