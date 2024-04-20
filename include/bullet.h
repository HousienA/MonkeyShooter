#ifndef BULLET_H
#define BULLET_H
#define BULLET_SPEED 0.01
#define BULLETLIFETIME 60
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

typedef struct {
    float x;
    float y;
    float dx; // direction vector components
    float dy;
    SDL_Texture *texture;
    //SDL_Renderer *renderer;
} Bullet;

Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY);
void destroyBullet(Bullet *bullet);
void moveBullet(Bullet *bullet);
void drawBullet(Bullet *bullet, SDL_Renderer *renderer);

#endif