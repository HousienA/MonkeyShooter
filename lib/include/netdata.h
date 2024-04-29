#ifndef NETDATA_H
#define NETDATA_H


#define MAX_MONKEYS 4


enum clientCommand{READY, UP, DOWN, LEFT, RIGHT, FIRE};
typedef enum clientCommand ClientCommand;


struct clientData{
   ClientCommand command;
   int playerNumber, slotsTaken[4];
};
typedef struct clientData ClientData;


struct bulletData{
   float x, y, dx, dy;
};
typedef struct bulletData BulletData;


struct monkeyData{
   float x, y, vx, vy;
   int health;
   BulletData bData;
};
typedef struct monkeyData MonkeyData;  


struct serverData{
   MonkeyData monkeys[MAX_MONKEYS];
   int playerNr, slotsTaken[4];
   //GameState gState;
   
};
typedef struct serverData ServerData;


#endif
