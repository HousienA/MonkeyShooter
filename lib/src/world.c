#include "../include/game.h"
#include "../include/character.h"
#include "../include/world.h"

//define the walls in map
Wall walls[23] = {
    {222, 252, 333, 505}, {252, 280, 290, 541}, {282, 315, 257, 541}, {315, 481, 213, 582},  //left side circle wall 
    {481, 518, 252, 545}, {518, 545, 288, 540}, {547, 562, 333, 508},    //circle wall

    {185, 234, 53, 109}, {24, 55, 139, 175}, {24, 64, 175, 222}, {54, 99, 333, 380},
    {156, 178, 422, 452}, {152, 184, 490, 525}, {209, 250, 659, 702}, {149, 208, 712, 770}, 
    {437, 458, 709, 738}, {577, 613, 603, 641}, {568, 621, 721, 780}, {700, 745, 329, 380},
    {578, 620, 247, 292}, {724, 762, 225, 278}, {736, 780, 130, 187}, {472, 526, 176, 206}       //trees
     
};

//check collision with boundaries and walls
bool checkCollision(Character *character, Wall *walls, int num_walls) {
    //check that its away from the borders
    if (character->dest.x < PLAYABLE_AREA_X_MIN || character->dest.x + character->dest.w > PLAYABLE_AREA_X_MAX ||
        character->dest.y < PLAYABLE_AREA_Y_MIN || character->dest.y + character->dest.h / 2 > PLAYABLE_AREA_Y_MAX) {
        return TRUE;   // true if collision with boundary is detected
    }
    
    //check if it's away from walls
    /*
    for (int i = 0; i < num_walls; ++i) {
        if (character->dest.x + character->dest.w > walls[i].x_min && character->dest.x < walls[i].x_max &&
            character->dest.y + character->dest.h > walls[i].y_min && character->dest.y + character->dest.h / 2 < walls[i].y_max) {
            return TRUE;    // true if collision with wall...
        }
    }
    */
    
    //no collision
    return FALSE;
}
