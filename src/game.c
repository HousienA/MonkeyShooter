#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/game.h"

int main(int argc, char *argv[])
{

    // initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("MonkeyShooter",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          PLAYER_WIDTH,
                                          PLAYER_HEIGHT,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);

    // LOad image file
    SDL_Surface *image = IMG_Load("resources/character.png");
    SDL_Texture *image_texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    // Define where on the screen we want the character
    SDL_Rect texture_destination;
    texture_destination.x = 100;
    texture_destination.y = 50;
    texture_destination.w = 250;
    texture_destination.h = 250;

    // Source Rectangle to cover image
    SDL_Rect scrR;
    scrR.x = 0;
    scrR.y = 0;
    scrR.w = PLAYER_WIDTH;
    scrR.h = PLAYER_HEIGHT;

    // Destination Rectangle: on screen
    SDL_Rect destR;
    destR.x = 100;
    destR.y = 50;
    destR.w = 64; // SIZE OF BOX
    destR.h = 64; // SIZE OF BOX ! CHANGE IF NEEDED

    int running = TRUE;
    SDL_Event event;
    while (running)
    {
        // Dynamically fetch the current window size
        int window_width, window_height;
        SDL_GetWindowSize(window, &window_width, &window_height);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = FALSE;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    destR.y = destR.y - MOVE_SPEED;
                    if (destR.y < 0)
                        destR.y = 0;
                    break;
                case SDLK_DOWN:
                    destR.y = destR.y + MOVE_SPEED;
                    if (destR.y + destR.h > window_height)
                        destR.y = window_height - destR.h;
                    break;
                case SDLK_LEFT:
                    destR.x = destR.x - MOVE_SPEED;
                    if (destR.x < 0)
                        destR.x = 0;
                    break;
                case SDLK_RIGHT:
                    destR.x = destR.x + MOVE_SPEED;
                    if (destR.x + destR.w > window_width)
                        destR.x = window_width - destR.w;
                    break;
                }
            }
        }

        // clear screen
        SDL_RenderClear(renderer);

        // draw
        SDL_RenderCopy(renderer, image_texture, NULL, &destR);

        // show what was drawn
        SDL_RenderPresent(renderer);
    }

    // Release resources
    SDL_DestroyTexture(image_texture);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
