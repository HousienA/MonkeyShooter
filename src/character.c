#include "../include/character.h"
#include <stdlib.h>
#include "../include/game.h"
#include "../include/world.h"
#include "../include/bullet.h"

Character *createCharacter(SDL_Renderer *renderer)
{
    Character *pCharacter = malloc(sizeof(Character));
    pCharacter->dest.x = 280;
    pCharacter->dest.y = 265;
    pCharacter->dest.w = 64;
    pCharacter->dest.h = 64;


    IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);
    SDL_Surface *image = IMG_Load("resources/Monkeyfront.png");


    pCharacter->tex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    pCharacter->source.x = 0;
    pCharacter->source.y = 0;
    pCharacter->source.w = PLAYER_WIDTH;
    pCharacter->source.h = PLAYER_HEIGHT;
    pCharacter->health = MAX_HEALTH;

    return pCharacter;
}

void decreaseHealth(Character *pCharacter){
    pCharacter->health-= 1;
    //if health is zero, character dies meaning destroy
    if(pCharacter->health<=0){
        pCharacter->health = 0;
        destroyCharacter(pCharacter);
    }
    
}

// Function to check if the character is alive
int isCharacterAlive(Character *pCharacter)
{
    return pCharacter->health > 0;
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

