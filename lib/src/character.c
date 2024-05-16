#include "../include/character.h"
#include <stdlib.h>
#include "../include/game.h"
#include "../include/world.h"
#include "../include/bullet.h"
#include "../include/netdata.h"

#define SPRITE_WIDTH 128
#define SPRITE_HEIGHT 128
#define ANIMATION_FRAMES 3

struct character{
    SDL_Rect dest;
    SDL_Rect source;
    SDL_Renderer *renderer;
    SDL_Texture *tex;
    Bullet *bullet[MAX_BULLETS];
    Wall *walls[23];
    int health;
    int currentFrame;
    Uint32 animationTimer;
    int direction; // 0 - down, 1 - left, 2 - right, 3 - up
    bool isHit;
    Uint32 hitTimer;
};


Character *createCharacter(SDL_Renderer *renderer, int characterNumber) {
    Character *pCharacter = malloc(sizeof(Character));
    pCharacter->dest.w = CHARACTER_WIDTH;
    pCharacter->dest.h = CHARACTER_HEIGHT;

    IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);
    SDL_Surface *image = IMG_Load("../lib/resources/SpriteMonkey.png");
    if (!image) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return FALSE;
    }

    switch(characterNumber){
        case 1:
            pCharacter->dest.x = 70;
            pCharacter->dest.y = 90;
            break;
        case 2:
            pCharacter->dest.x = 699;
            pCharacter->dest.y = 90;
            break;
    }

    pCharacter->tex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    pCharacter->source.x = 0;
    pCharacter->source.y = 0;
    pCharacter->source.w = SPRITE_WIDTH;
    pCharacter->source.h = SPRITE_HEIGHT;

    pCharacter->health = MAX_HEALTH;
    pCharacter->currentFrame = 0;
    pCharacter->animationTimer = 0;
    pCharacter->direction = 0;
    pCharacter->isHit = FALSE;
    pCharacter->hitTimer = 0;

    return pCharacter;
}

void updateCharacterAnimation(Character *pCharacter, Uint32 deltaTime)
{
    pCharacter->animationTimer += deltaTime;
    // Change sprite frame every 400 milliseconds
    if (pCharacter->animationTimer > 400)
    {
        pCharacter->currentFrame = (pCharacter->currentFrame + 1) % ANIMATION_FRAMES;
        pCharacter->animationTimer = 0;
    }

    //sprite row based on movement direction
    switch (pCharacter->direction) {
        case 1: // Left
            pCharacter->source.y = SPRITE_HEIGHT * 2;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
        case 2: // Right
            pCharacter->source.y = SPRITE_HEIGHT;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
        case 3: // Up
            pCharacter->source.y = SPRITE_HEIGHT * 3;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
        default: // Down
            pCharacter->source.y = 0;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
    }
}

void renderCharacter(Character *pCharacter, SDL_Renderer *renderer) {
    if(!pCharacter->health) return;

    Uint8 r, g, b;
    Uint32 currentTime = SDL_GetTicks();
    SDL_GetTextureColorMod(pCharacter->tex, &r, &g, &b);
    
    if(pCharacter->isHit && currentTime - pCharacter->hitTimer < 200){
        SDL_SetTextureColorMod(pCharacter->tex, 255, 0, 0);
    } else {
        pCharacter->isHit = FALSE;
        SDL_SetTextureColorMod(pCharacter->tex, 255, 255, 255);
    }
    
    SDL_Rect dest = {pCharacter->dest.x, pCharacter->dest.y, pCharacter->dest.w, pCharacter->dest.h};
    SDL_RenderCopy(renderer, pCharacter->tex, &pCharacter->source, &pCharacter->dest);
}

void destroyCharacter(Character *pCharacter) {
    SDL_DestroyTexture(pCharacter->tex);
    free(pCharacter);
}

void decreaseHealth(Character *pCharacter) {
    pCharacter->health -= 1;
    printf("Health: %d\n",pCharacter->health);
    pCharacter->isHit = TRUE;
    pCharacter->hitTimer = SDL_GetTicks();
    if(pCharacter->isHit){
        SDL_SetTextureColorMod(pCharacter->tex, 255, 0, 0);
    }
    
    if (pCharacter->health == 0) {
        destroyCharacter(pCharacter);
    }
}

int isCharacterAlive(Character *pCharacter) {
    return pCharacter->health > 0;
}

void turnLeft(Character *pCharacter) {
    pCharacter->dest.x -= MOVE_SPEED;
    pCharacter->direction = 1;
    updateCharacterAnimation(pCharacter, 100); 
}

void turnRight(Character *pCharacter) {
    pCharacter->dest.x += MOVE_SPEED;
    pCharacter->direction = 2;
    updateCharacterAnimation(pCharacter, 100);
}

void turnUp(Character *pCharacter) {
    pCharacter->dest.y -= MOVE_SPEED;
    pCharacter->direction = 3;
    updateCharacterAnimation(pCharacter, 100);
}

void turnDown(Character *pCharacter) {
    pCharacter->dest.y += MOVE_SPEED;
    pCharacter->direction = 0;
    updateCharacterAnimation(pCharacter, 100);
}

void characterSendData(Character *pCharacter, MonkeyData *pMonkeyData) {
    pMonkeyData->x = pCharacter->dest.x;
    pMonkeyData->y = pCharacter->dest.y;
    pMonkeyData->sx = pCharacter->source.x;
    pMonkeyData->sy = pCharacter->source.y;
    pMonkeyData->health = pCharacter->health;
    //SendBulletData(pCharacter->bullet, &pMonkeyData->bData);
}

void updateCharacterFromServer(Character *pCharacter, MonkeyData *monkeys) {
    pCharacter->dest.x = monkeys->x;
    pCharacter->dest.y = monkeys->y;
    pCharacter->source.x = monkeys->sx;
    pCharacter->source.y = monkeys->sy;
    pCharacter->health = monkeys->health;
    //updateBulletFromServer(pCharacter->bullet, &pMonkeyData->bData);
}

void healthBar(Character *pCharacter, SDL_Renderer *renderer) {
    SDL_Rect healthBar = {20, 20, 100, 20}; 
    SDL_Rect remainingHealth = {20, 20, pCharacter->health * 25, 20}; 
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &healthBar); 
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
    SDL_RenderFillRect(renderer, &remainingHealth); 
}

bool checkCollisionCharacterBullet(Character *pCharacter, Bullet *bullet) {
    SDL_Rect characterRect = {pCharacter->dest.x, pCharacter->dest.y, CHARACTER_WIDTH, CHARACTER_HEIGHT};
    SDL_Rect bulletRect = getBulletRect(bullet);

    return SDL_HasIntersection(&characterRect, &bulletRect);
}

void setBulletStartPosition(Character *pCharacter, float *startX, float *startY) {
    *startX = pCharacter->dest.x + pCharacter->dest.w / 2;
    *startY = pCharacter->dest.y + pCharacter->dest.h / 2;
}

bool checkCollision(Character *character, Wall *walls, int num_walls) {
    if (character->dest.x < PLAYABLE_AREA_X_MIN || character->dest.x + character->dest.w > PLAYABLE_AREA_X_MAX ||
        character->dest.y < PLAYABLE_AREA_Y_MIN || character->dest.y + character->dest.h / 2 > PLAYABLE_AREA_Y_MAX) {
        return TRUE;   // true if collision with boundary is detected        
    }
    int margin = 10; 
    for (int i = 0; i < num_walls; ++i) {
        if (character->dest.x + character->dest.w - margin > walls[i].x_min && character->dest.x + margin < walls[i].x_max &&
            character->dest.y + character->dest.h > walls[i].y_min && character->dest.y + character->dest.h / 2 < walls[i].y_max) {
            return TRUE;    // true if collision with wall...
        }
    }

    return FALSE;
}