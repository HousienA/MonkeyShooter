#include "../include/character.h"
#include <stdlib.h>
#include "../include/game.h"


Character *createCharacter(SDL_Renderer *renderer)
{
    Character *pCharacter = malloc(sizeof(Character));


    pCharacter->moving_left = false;
    pCharacter->moving_right = false;
    pCharacter->moving_up = false;
    pCharacter->moving_down = false;
    pCharacter->dest.x = 200;
    pCharacter->dest.y = 550;
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
    if (pCharacter->dest.x - MOVE_SPEED >= 140 &&
        (pCharacter->dest.x - MOVE_SPEED > 520 || pCharacter->dest.y > 430 || pCharacter->dest.y < 130) &&
        (pCharacter->dest.x - MOVE_SPEED > 345 || pCharacter->dest.x - MOVE_SPEED + CHARACTER_WIDTH < 275 || pCharacter->dest.y + CHARACTER_HEIGHT < 375))
        pCharacter->dest.x -= MOVE_SPEED;
}

void turnRight(Character *pCharacter)
{
    if (pCharacter->dest.x + MOVE_SPEED + CHARACTER_WIDTH <= 660 &&
        (pCharacter->dest.x + MOVE_SPEED + CHARACTER_WIDTH < 445 || pCharacter->dest.y > 430 || pCharacter->dest.y < 130) &&
        (pCharacter->dest.x + MOVE_SPEED > 345 || pCharacter->dest.x + MOVE_SPEED + CHARACTER_WIDTH < 275 || pCharacter->dest.y + CHARACTER_HEIGHT < 375))
        pCharacter->dest.x += MOVE_SPEED;
}

void turnUp(Character *pCharacter)
{
    if (pCharacter->dest.y - MOVE_SPEED >= 130 &&
        (pCharacter->dest.y - MOVE_SPEED > 430 || pCharacter->dest.x + CHARACTER_WIDTH < 445 || pCharacter->dest.x > 520))
        pCharacter->dest.y -= MOVE_SPEED;
}

void turnDown(Character *pCharacter)
{
    if (pCharacter->dest.y + MOVE_SPEED + CHARACTER_HEIGHT <= 650 &&
        (pCharacter->dest.y + MOVE_SPEED + CHARACTER_HEIGHT < 375 || pCharacter->dest.x + CHARACTER_WIDTH < 275 || pCharacter->dest.x > 345))
        pCharacter->dest.y += MOVE_SPEED;
}


void destroyCharacter(Character *pCharacter)
{
    SDL_DestroyTexture(pCharacter->tex);
    free(pCharacter);
}