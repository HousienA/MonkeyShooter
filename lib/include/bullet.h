#ifndef BULLET_H
#define BULLET_H
#define BULLET_SPEED 5
#define BULLETLIFETIME 60
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "../include/game.h"
#include "../include/character.h"

#define BULLET_WIDTH 5
#define BULLET_HEIGHT 5
typedef struct {
    float x;
    float y;
    float dx; 
    float dy;
    int whoShot;
    SDL_Texture *texture;
} Bullet;

Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY, int whoShot);
void destroyBullet(Bullet *bullet);
void moveBullet(Bullet *bullet);
void drawBullet(Bullet *bullet, SDL_Renderer *renderer);
float xBullet(Bullet *bullet);
float DxBullet(Bullet *bullet);
float yBullet(Bullet *bullet);
float DyBullet(Bullet *bullet);
//bool checkCollisionBulletCharacter(Bullet *bullet, Character *pCharacter);

#endif