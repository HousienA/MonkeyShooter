#include <stdio.h>
#include <SDL2/SDL.h>

#include "./constants.h"

int game_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;


int initializeWindow(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }
    window = SDL_CreateWindow(
        "MonkeyShooter", 
        SDL_WINDOWPOS_CENTERED, // x värdet (put in the center with sdl)
        SDL_WINDOWPOS_CENTERED, // y värdet
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
        );
        if(!window){
            fprintf(stderr, "Error creating SDL window.\n");
            return FALSE;
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(!renderer){
            fprintf(stderr, "Error creating SDL renderer.\n");
            return FALSE;
        }

    return TRUE;
}

void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
        case SDL_QUIT:
            game_running = FALSE;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE){
                game_running = FALSE;
            }
            break;

    }

}

void update(){
    // Att göra 

}

void render(){
    // Att göra 

}

void destroy_window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main(){

    game_running = initializeWindow();

    //setup();

    while(game_running){
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}