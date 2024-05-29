#include "../include/game.h"
#include "../include/character.h"
#include "../include/world.h"

//define the walls in map
Wall walls[23] = {
    {230, 252, 333, 505}, {252, 280, 290, 541}, {282, 315, 257, 541}, {315, 481, 217, 582},  //left side circle wall 
    {481, 518, 252, 545}, {518, 545, 288, 540}, {547, 562, 333, 508},    //circle wall

    {185, 234, 53, 109}, {24, 55, 139, 175}, {24, 64, 175, 222}, {54, 99, 333, 380},
    {156, 178, 422, 452}, {152, 184, 490, 525}, {209, 250, 659, 702}, {149, 208, 712, 770}, 
    {437, 458, 709, 738}, {577, 613, 603, 641}, {568, 621, 721, 780}, {700, 745, 329, 380},
    {578, 620, 247, 292}, {724, 762, 225, 278}, {736, 780, 130, 187}, {472, 526, 176, 206}       //trees
     
};

void convertWallsToRects(Wall *walls, SDL_Rect *rects, int count) {
    //walls rects to be used for bullet collision
    for (int i = 0; i < count; ++i) {
        rects[i].x = walls[i].x_min;
        rects[i].y = walls[i].y_min;
        rects[i].w = walls[i].x_max - walls[i].x_min;      //get width and height 
        rects[i].h = walls[i].y_max - walls[i].y_min;
    }
}