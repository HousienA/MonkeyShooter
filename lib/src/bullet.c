#include "../include/character.h"
#include "../include/bullet.h"
#include  "../include/netdata.h"
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
    bullet->lifeTime = SDL_GetTicks();
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


bool checkCollisionBulletCharacter(Bullet *bullet, Character *pCharacter) {
    // Calculate the bounding box for the bullet
    float bulletLeft = bullet->x;
    float bulletRight = bullet->x + 5; // Assuming bullet width is 5
    float bulletTop = bullet->y;
    float bulletBottom = bullet->y + 5; // Assuming bullet height is 5

    // Calculate the bounding box for the character
    int characterLeft = pCharacter->dest.x;
    int characterRight = pCharacter->dest.x + CHARACTER_WIDTH;
    int characterTop = pCharacter->dest.y;
    int characterBottom = pCharacter->dest.y - CHARACTER_HEIGHT;
    printf("Bullet: %d %d %d %d\n", bulletLeft, bulletRight, bulletTop, bulletBottom);

    // Check for collision
    if (bulletRight >= characterLeft && bulletLeft <= characterRight &&
        bulletBottom >= characterTop && bulletTop <= characterBottom) {
        // Collision detected
        return true;
    }
    return false;

    
}

void SendDataBullet(Bullet *bullet, BulletData *pBullet) {
    pBullet->x = bullet->x;
    pBullet->y = bullet->y;
    pBullet->dx = bullet->dx;
    pBullet->dy = bullet->dy;
}

void updateBulletFromServer(Bullet *bullet, BulletData *pBullet) {
    bullet->x = pBullet->x;
    bullet->y = pBullet->y;
    bullet->dx = pBullet->dx;
    bullet->dy = pBullet->dy;
}
