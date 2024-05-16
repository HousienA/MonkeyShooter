#include "../include/character.h"
#include "../include/bullet.h"
#include "../include/character.h"
#include "../include/world.h"
#include "../include/netdata.h"
#include <stdlib.h>
#include <math.h>

Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY, int whoShot) {
    Bullet *bullet = malloc(sizeof(Bullet));
    if (!bullet) {
        printf("Failed to allocate memory for bullet.\n");
        return NULL;
    }
    bullet->x = startX;
    bullet->y = startY;
    bullet->dx = 0; 
    bullet->dy = 0;
    bullet->whoShot = whoShot;

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

SDL_Rect getBulletRect(Bullet *bullet) {
    SDL_Rect rect = {bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT};
    return rect;
}

void drawBullet(Bullet *bullet, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color 
    SDL_Rect rect = getBulletRect(bullet);
    SDL_RenderFillRect(renderer, &rect); // Draw a bullet
}

float xBullet(Bullet *bullet){
    return bullet->x;
}

float DxBullet(Bullet *bullet){
    return bullet->dx;
}

float yBullet(Bullet *bullet){
    return bullet->y;
}

float DyBullet(Bullet *bullet){
    return bullet->dy;
}

bool checkCollisionBulletWall(Bullet *bullet, Wall *walls, int num_walls) {
    SDL_Rect bulletRect = getBulletRect(bullet);
    SDL_Rect wallRects[num_walls];
    
    convertWallsToRects(walls, wallRects, num_walls);    //get wall rects

    for (int i = 0; i < num_walls; i++) {
        if (SDL_HasIntersection(&bulletRect, &wallRects[i])) {
            return true;
        }
    }
    return false;
}
