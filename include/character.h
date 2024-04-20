#ifndef CHARACTER_H
#define CHARACTER_H

#define CHARACTER_HEIGHT 64
#define CHARACTER_WIDTH 64
#define MAX_HEALTH 4

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
    SDL_Rect dest;
    SDL_Rect source;
    SDL_Renderer *renderer;
    SDL_Texture *tex;
    int health;

} Character;

Character *createCharacter(SDL_Renderer *renderer);
void turnLeft(Character *pCharacter);
void turnRight(Character *pCharacter);
void turnUpp(Character *pCharacter);
void turnDown(Character *pCharacter);
void destroyCharacter(Character *pCharacter);
void handleShooting(Character *pCharacter);

#endif 