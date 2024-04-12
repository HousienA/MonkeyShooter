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
    bool moving_left;
    bool moving_right;
    bool moving_up;
    bool moving_down;
};

typedef struct ctx CTX;

void movement()
{
    CTX ctx;

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
        SDL_RenderCopy(ctx.renderer, ctx.player.tex, &ctx.player.source, &ctx.player.dest);
        SDL_RenderPresent(ctx.renderer);
    }

}