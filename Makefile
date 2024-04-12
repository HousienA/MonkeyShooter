# Makefile for Windows

# Location of main.c
SRCDIR=./src

# Name of the compiler
CC=gcc

# Any include directories
# INCLUDE = ./lib/macos/include
INCLUDE = C:\msys64\mingw64\include

# Any flags for the compiler
CFLAGS = -g -c

# Any libraries to link with
#LIBS = C:\msys64\mingw64\lib
#...

# Extra flags to give to compilers when they are supposed to invoke the linker
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -mwindows -lm

main:
	@echo "Building main"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c -o $@.o 
	$(CC) main.o -o main $(LDFLAGS)

clean:
	rm main
	rm main.o
