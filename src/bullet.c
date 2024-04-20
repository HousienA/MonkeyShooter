#include "../include/bullet.h"
#include "../include/character.h"
#include <stdlib.h>
#include <math.h>

Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY) {
    Bullet *bullet = malloc(sizeof(Bullet));
    if (!bullet) {
        printf("Failed to allocate memory for bullet.\n");
        return NULL;
    }
    bullet->x = startX;
    bullet->y = startY;
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