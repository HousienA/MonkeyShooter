#include "../include/character.h"
#include "../include/bullet.h"
#include <stdlib.h>
#include <math.h>

Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY) {
    Bullet *bullet = malloc(sizeof(Bullet));
    if (!bullet) {
        printf("Failed to allocate memory for bullet.\n");
        return NULL;
    }
    bullet->x = startX + 32;
    bullet->y = startY + 32;
    bullet->dx = 0; 
    bullet->dy = 0;

    return bullet;
}

void destroyBullet(Bullet *bullet) {
    if (bullet) {
        free(bullet);
    }
}

void moveBullet(Bullet *bullet) {
    bullet->x += bullet->dx;
    bullet->y += bullet->dy;
}

void drawBullet(Bullet *bullet, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color 
    SDL_Rect rect = {bullet->x, bullet->y, 5, 5};
    SDL_RenderFillRect(renderer, &rect); // Draw a bullet
}


bool checkCollisionBulletCharacter(Bullet *bullet, Character *pCharacter){
    int bx1 = bullet->x;
    int bx2 = bullet->x + BULLET_WIDTH;
    int by1 = bullet->y;
    int by2 = bullet->y + BULLET_HEIGHT;

    int cx1 = pCharacter->dest.x;
    int cx2 = pCharacter->dest.x + CHARACTER_WIDTH;
    int cy1 = pCharacter->dest.y;
    int cy2 = pCharacter->dest.y + CHARACTER_HEIGHT;

    if (bx2 < cx1 || bx1 > cx2 || by2 < cy1 || by1 > cy2) {
        // No collision
        return false;
    } else {
        // Collision detected
        return true;
    }
}

