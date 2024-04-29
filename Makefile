INCLUDE = C:\msys64\mingw64\include
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -lm
CFLAGS = -Wall

main: main.o character.o world.o bullet.o
	gcc -o main main.o character.o world.o bullet.o $(LDFLAGS)

main.o: ./lib/src/main.c
	gcc -c -I$(INCLUDE) ./lib/src/main.c

character.o: ./lib/src/character.c
	gcc -c -I$(INCLUDE) ./lib/src/character.c

world.o: ./lib/src/world.c
	gcc -c -I$(INCLUDE) ./lib/src/world.c

bullet.o: ./lib/src/bullet.c
	gcc -c -I$(INCLUDE) ./lib/src/bullet.c


clean:
	rm *.o
	rm *.exe