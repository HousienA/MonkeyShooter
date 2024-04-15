INCLUDE = C:\msys64\mingw64\include
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -lm

main: main.o character.o world.o
	gcc -o main main.o character.o world.o $(LDFLAGS)

main.o: ./src/main.c
	gcc -c -I$(INCLUDE) ./src/main.c

character.o: ./src/character.c
	gcc -c -I$(INCLUDE) ./src/character.c

world.o: ./src/world.c
	gcc -c -I$(INCLUDE) ./src/world.c

clean:
	rm *.o
	rm *.exe