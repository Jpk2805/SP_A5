CC = gcc
CFLAGS = -I./inc -c -Wall
LDFLAGS = -lrt -lpthread

all: DP-2

DP-2: obj/dp2.o
	$(CC) -o bin/DP-2 obj/dp-2.o $(LDFLAGS)

obj/dp2.o: src/dp2.cpp
	$(CC) $(CFLAGS) src/dp2.cpp -o obj/dp2.o

clean:
	rm -rf ./bin/DP-2 ./obj/*.o

