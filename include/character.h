#ifndef CHARACTER_H
#define CHARACTER_H

#define CHARACTER_HEIGHT 64
#define CHARACTER_WIDTH 64

#define WALL1_X1 445
#define WALL1_X2 520 //
#define WALL1_Y1 430
#define WALL1_Y2 130

#define WALL2_X1 275
#define WALL2_X2 345 //
#define WALL2_Y1 375
#define WALL2_Y2 650 

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

Character *createCharacter();
void turnLeft(Character *pCharacter);
void turnRight(Character *pCharacter);
void turnUp(Character *pCharacter);
void turnDown(Character *pCharacter);
void destroyCharacter(Character *pCharacter);

#endif 