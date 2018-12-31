PROGRAM = xd
CC = gcc
CFLAGS = -Wall -std=c99 -lm
LDFLAGS = -lGLU -lglut -lGL 


$(PROGRAM): src/main.o src/vecops.o src/input.o src/camera.o
	$(CC) src/main.o src/vecops.o src/input.o src/camera.o -o $(PROGRAM) $(CFLAGS) $(LDFLAGS)


main.o: src/main.c 
	$(CC) $(CFLAGS) src/main.c

vecops.o: src/vecops.c vecops.h
	$(CC) $(CFLAGS) src/vecops.c

input.o: src/input.c src/input.h
	$(CC) $(CFLAGS) src/input.c

camera.o: src/camera.c src/camera.h
	$(CC) $(CFLAGS) src/camera.c

.PHONY: clean

clean: 
	rm *.o
