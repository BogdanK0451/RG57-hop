PROGRAM = xd
CC = gcc
CFLAGS = -Wall -std=c99 -lm
LDFLAGS = -lGLU -lglut -lGL 


$(PROGRAM): src/main.o src/vecops.o src/input.o src/camera.o src/loader.o external/re.o 
	$(CC) src/main.o src/vecops.o src/input.o src/camera.o src/loader.o external/re.o   -o $(PROGRAM) $(CFLAGS) $(LDFLAGS)


main.o: src/main.c src/input.h src/camera.h src/loader.h 
	$(CC) $(CFLAGS) src/main.c

loader.o: src/loader.c src/loader.h external/re.h external/stb_image.h
	$(CC) $(CFLAGS) src/loader.c

vecops.o: src/vecops.c src/vecops.h
	$(CC) $(CFLAGS) src/vecops.c

input.o: src/input.c src/input.h src/camera.h src/loader.h
	$(CC) $(CFLAGS) src/input.c

camera.o: src/camera.c src/camera.h
	$(CC) $(CFLAGS) src/camera.c

re.o: external/re.c external/re.h
	$(CC) $(CFLAGS) external/re.c

	
.PHONY: clean

clean: 
	rm src/*.o
