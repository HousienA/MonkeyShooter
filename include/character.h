#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
    bool moving_left;
    bool moving_right;
    bool moving_up;
    bool moving_down;
    SDL_Rect dest;
    SDL_Rect source;
    SDL_Renderer *renderer;
    SDL_Texture *tex;
} Character;

Character *createCharacter(SDL_Renderer *renderer);
void turnLeft(Character *pCharacter);
void turnRight(Character *pCharacter);
void turnUpp(Character *pCharacter);
void turnDown(Character *pCharacter);
void destroyCharacter(Character *pCharacter);

#endif 