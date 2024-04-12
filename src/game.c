#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/game.h"
#include "../include/constants.h"

struct entity{
    SDL_Texture *tex;
    SDL_Rect source;
    SDL_Rect dest;
};

typedef struct entity Entity;

struct ctx{
    SDL_Window *window;
    SDL_Renderer *renderer;

    Entity player;
    Entity background;
    bool moving_left;
    bool moving_right;
    bool moving_up;
    bool moving_down;
};

typedef struct ctx CTX;

int main(int argc, char *argv[])
{

    CTX ctx;
    // initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    ctx.window = SDL_CreateWindow("MonkeyShooter",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    ctx.renderer = SDL_CreateRenderer(ctx.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);

    // LOad image file
    SDL_Surface *image = IMG_Load("resources/character.png");
    ctx.player.tex = SDL_CreateTextureFromSurface(ctx.renderer, image);
    SDL_FreeSurface(image);

    // Define where on the screen we want the character
    ctx.player.dest.x = 100;
    ctx.player.dest.y = 50;
    ctx.player.dest.w = 64;
    ctx.player.dest.h = 64;

    // Source Rectangle to cover image
    ctx.player.source.x = 0;
    ctx.player.source.y = 0;
    ctx.player.source.w = PLAYER_WIDTH;
    ctx.player.source.h = PLAYER_HEIGHT;

    // Load background image file
    SDL_Surface *background_image = IMG_Load("resources/PrototypeMap.MS2.png");
    ctx.background.tex = SDL_CreateTextureFromSurface(ctx.renderer, background_image);
    SDL_FreeSurface(background_image);

    // Define where on the screen we want the background
    ctx.background.dest.x = 0;
    ctx.background.dest.y = 0;
    ctx.background.dest.w = WINDOW_WIDTH;
    ctx.background.dest.h = WINDOW_HEIGHT;


    int running = TRUE;
    SDL_Event event;
    const Uint8 *state;
    while (running) {
        state = SDL_GetKeyboardState(NULL);

        ctx.moving_left = state[SDL_SCANCODE_A] > 0;
        ctx.moving_right = state[SDL_SCANCODE_D] > 0;
        ctx.moving_up = state[SDL_SCANCODE_W] > 0;
        ctx.moving_down = state[SDL_SCANCODE_S] > 0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = FALSE;
            }
        }

        if (ctx.moving_left && ctx.player.dest.x > 0) {
            ctx.player.dest.x -= MOVE_SPEED;
        }

        if (ctx.moving_right && ctx.player.dest.x + ctx.player.dest.w < WINDOW_WIDTH) {
            ctx.player.dest.x += MOVE_SPEED;
        }

        if (ctx.moving_up && ctx.player.dest.y > 0) {
            ctx.player.dest.y -= MOVE_SPEED;
        }

        if (ctx.moving_down && ctx.player.dest.y + ctx.player.dest.h < WINDOW_HEIGHT) {
            ctx.player.dest.y += MOVE_SPEED;
        }


        SDL_RenderClear(ctx.renderer);
        SDL_RenderCopy(ctx.renderer, ctx.background.tex, NULL, &ctx.background.dest);
        SDL_RenderCopy(ctx.renderer, ctx.player.tex, &ctx.player.source, &ctx.player.dest);

        SDL_RenderPresent(ctx.renderer);
    }

    // Release resources
    SDL_DestroyTexture(ctx.player.tex);
    SDL_DestroyTexture(ctx.background.tex);
    IMG_Quit();
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();

    return 0;
}