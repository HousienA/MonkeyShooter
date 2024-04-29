#ifndef NETDATA_H
#define NETDATA_H


#define MAX_MONKEYS 4


enum clientCommand{READY, UP, DOWN, LEFT, RIGHT, FIRE};
typedef enum clientCommand ClientCommand;

enum gameState {MENU, ONGOING };
typedef enum gameState GameState;
struct monkeyData{
   float x, y, vx, vy;
   int health;
   
};
typedef struct monkeyData MonkeyData;
struct clientData{
   ClientCommand command;
   MonkeyData monkey;
   int playerNumber, slotsTaken[4];
   int numberOfBullets;
};
typedef struct clientData ClientData;


struct bulletData{
   float x, y, dx, dy;
};
typedef struct bulletData BulletData;


  


struct serverData{
   MonkeyData monkeys[MAX_MONKEYS];
   int slotsTaken[4], numberOfBullets;
   GameState gState;
   BulletData bData[1000];
};
typedef struct serverData ServerData;


#endif