#ifndef WORLD_H
#define WORLD_H

#include "../include/character.h"
#include "../include/bullet.h"

//defines coordinates and areas in map
#define MAP_WIDTH 800
#define MAP_HEIGHT 800
#define PLAYABLE_AREA_X_MIN 140
#define PLAYABLE_AREA_X_MAX 650
#define PLAYABLE_AREA_Y_MIN 130
#define PLAYABLE_AREA_Y_MAX 645

//struct for wall creation
typedef struct {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
} Wall;

//make struct accesible to other files. (can be modified as a pointed for easier reach later)
extern Wall walls[2];

//function to check collision with character
bool checkCollision(Character *character, Wall *walls, int num_walls);
//bool checkCollisionWithBullet(Character *character, Bullet *bullet);

#endif // WORLD_H