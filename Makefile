INCLUDE = C:\msys64\mingw64\include
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -lm

game: game.o character.o
	gcc -o game game.o character.o $(LDFLAGS)

game.o: ./src/game.c
	gcc -c -I$(INCLUDE) ./src/game.c

character.o: ./src/character.c
	gcc -c -I$(INCLUDE) ./src/character.c

clean:
	rm .o
	rm game.exe