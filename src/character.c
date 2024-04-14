#include "../include/character.h"
#include <stdlib.h>
#include "../include/game.h"


Character *createCharacter(SDL_Renderer *renderer)
{
    Character *pCharacter = malloc(sizeof(Character));
    pCharacter->dest.x = 100;
    pCharacter->dest.y = 50;
    pCharacter->dest.w = 64;
    pCharacter->dest.h = 64;

    IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);
    SDL_Surface *image = IMG_Load("resources/character.png");


    pCharacter->tex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    pCharacter->source.x = 0;
    pCharacter->source.y = 0;
    pCharacter->source.w = PLAYER_WIDTH;
    pCharacter->source.h = PLAYER_HEIGHT;

    return pCharacter;
}

void turnLeft(Character *pCharacter)
{
    pCharacter->dest.x -= MOVE_SPEED;
}

void turnRight(Character *pCharacter)
{
    pCharacter->dest.x += MOVE_SPEED;
}

void turnUpp(Character *pCharacter)
{
    pCharacter->dest.y -= MOVE_SPEED;
}

void turnDown(Character *pCharacter)
{
    pCharacter->dest.y += MOVE_SPEED;
}

void destroyCharacter(Character *pCharacter)
{
    SDL_DestroyTexture(pCharacter->tex);
    free(pCharacter);
}