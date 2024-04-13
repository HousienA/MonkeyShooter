INCLUDE = C:\msys64\mingw64\include
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -lm

main: main.o character.o
	gcc -o main main.o character.o $(LDFLAGS)

main.o: ./src/main.c
	gcc -c -I$(INCLUDE) ./src/main.c

character.o: ./src/character.c
	gcc -c -I$(INCLUDE) ./src/character.c

clean:
	rm *.o
	rm *.exe