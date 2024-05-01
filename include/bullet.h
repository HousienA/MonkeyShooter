#ifndef bullet_h
#define bullet_h

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct bullet Bullet;


#define BULLET_SPEED 5
#define BULLETLIFETIME 60

#define BULLET_WIDTH 5
#define BULLET_HEIGHT 5


Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY);
void destroyBullet(Bullet *bullet);
void moveBullet(Bullet *bullet);
void drawBullet(Bullet *bullet, SDL_Renderer *renderer);
bool checkCollisionBulletCharacter(Bullet *bullet, Character *pCharacter);

#endif