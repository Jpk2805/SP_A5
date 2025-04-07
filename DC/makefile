CC = gcc
CFLAGS =  -I./inc -c -Wall
LDFLAGS = -lrt -pthread

all: DC

DC: obj/dc.o
	$(CC) -o bin/DC obj/dc.o $(LDFLAGS)

obj/dc.o: src/dc.cpp
	$(CC) $(CFLAGS) src/dc.cpp -o obj/dc.o 

clean:
	rm -rf ./bin/DC ./obj/*.o
