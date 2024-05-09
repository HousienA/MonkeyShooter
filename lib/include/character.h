#ifndef CHARACTER_H
#define CHARACTER_H

#define CHARACTER_HEIGHT 46
#define CHARACTER_WIDTH 46
#define MAX_HEALTH 4

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "netdata.h"
#include "bullet.h"


typedef struct {
    SDL_Rect dest;
    SDL_Rect source;
    SDL_Renderer *renderer;
    SDL_Texture *tex;
    Bullet *bullet[MAX_BULLETS];
    int health;
    int currentFrame;
    Uint32 animationTimer;
    int direction; // 0 - down, 1 - left, 2 - right, 3 - up
} Character;

Character *createCharacter(SDL_Renderer *renderer);
void decreaseHealth(Character *pCharacter);
int isCharacterAlive(Character *pCharacter);
void turnLeft(Character *pCharacter);
void turnRight(Character *pCharacter);
void turnUp(Character *pCharacter);
void turnDown(Character *pCharacter);
void updateCharacterAnimation(Character *pCharacter, Uint32 deltaTime);
void renderCharacter(Character *pCharacter, SDL_Renderer *renderer);
void destroyCharacter(Character *pCharacter);
void characterSendData(Character *pCharacter, MonkeyData *pMonkeyData);
void updateCharacterFromServer(Character *pCharacter, MonkeyData *pMonkeyData);
void healthBar(Character *pCharacter, SDL_Renderer *renderer);
bool checkCollisionCharacterBullet(Character *pCharacter, Bullet *bullet);
void bulletCreate(Bullet *bullet, Character *pCharacter);

#endif
