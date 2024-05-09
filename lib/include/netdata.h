#ifndef NETDATA_H
#define NETDATA_H


#define MAX_MONKEYS 2
#define MAX_BULLETS 100



enum clientCommand{READY, UP, DOWN, LEFT, RIGHT, FIRE, BLOCKED};
typedef enum clientCommand ClientCommand;

enum gameState {MENU, ONGOING };
typedef enum gameState GameState;

struct monkeyData{
   float x, y, sx, sy;
   int health;
   
};
typedef struct monkeyData MonkeyData;
struct clientData{
   ClientCommand command[7];
   MonkeyData monkey;
   int playerNumber, slotsTaken[4];
   int numberOfBullets;
   float bulletStartX[MAX_MONKEYS], bulletStartY[MAX_MONKEYS], bulletDx[MAX_MONKEYS], bulletDy[MAX_MONKEYS];
};
typedef struct clientData ClientData;

struct bulletData{
   float x, y, dx, dy;
};
typedef struct bulletData BulletData;


struct serverData{
   MonkeyData monkeys[MAX_MONKEYS];
   ClientCommand fire;
   int slotsTaken[4], numberOfBullets, numberOfPlayers, whoShot;
   GameState gState;
   float bulletStartX, bulletStartY, bulletDx, bulletDy;
   
};
typedef struct serverData ServerData;


#endif
