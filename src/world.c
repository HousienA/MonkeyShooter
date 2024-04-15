#include "../include/game.h"
#include "../include/character.h"
#include "../include/world.h"

//define the walls in map
Wall walls[2] = {
    {275, 345, 375, 645}, //wall 1
    {445, 520, 130, 430}  //wall 2
};

//check collision with boundaries and walls
bool checkCollision(Character *character, Wall *walls, int num_walls) {
    //check that its away from the borders
    if (character->dest.x < PLAYABLE_AREA_X_MIN || character->dest.x + character->dest.w > PLAYABLE_AREA_X_MAX ||
        character->dest.y < PLAYABLE_AREA_Y_MIN || character->dest.y + character->dest.h > PLAYABLE_AREA_Y_MAX) {
        return TRUE;    // true if collision with boundary is detected
    }

    //check if it's away from walls
    for (int i = 0; i < num_walls; ++i) {
        if (character->dest.x + character->dest.w > walls[i].x_min && character->dest.x < walls[i].x_max &&
            character->dest.y + character->dest.h > walls[i].y_min && character->dest.y < walls[i].y_max) {
            return TRUE;    // true if collision with wall...
        }
    }

    //no collision
    return FALSE;
}
