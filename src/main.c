#include <stdio.h>
#include <SDL2/SDL.h>
#include "../include/constants.h"

int game_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

//main struct for game
struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    //character struct called from character.h goes here *pCharacter;
};
typedef struct game Game;

int intializeWindow(Game *pGame);
void process_input(Game *pGame,SDL_Event *pEvent);
void run(Game *pGame);
void close(Game *pGame);


int main(int argv, char** args){
    Game g={0};
    if(!intializeWindow(&g)) return 1;      // if initializeWindow doesn't work end the program
    run(&g);            //or run and then close it after quitting
    close(&g);

    return 0;
}

//start the program and call needed from main struct
int intializeWindow(Game *pGame){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error: %s\n",SDL_GetError());
        return FALSE;
    }
    pGame->pWindow = SDL_CreateWindow(
        "MonkeyShooter", 
        SDL_WINDOWPOS_CENTERED, // x värdet (put in the center with sdl)
        SDL_WINDOWPOS_CENTERED, // y värdet
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
        );
        if(!pGame->pWindow){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return FALSE;
        }
        pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, 0);
        if(!pGame->pRenderer){
            printf("Error: %s\n",SDL_GetError());
            return FALSE;
        }
        /*pGame->pCharacter = createChracter()
        if(!pGame->pCharacter){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }*/
    return TRUE;
}

//input to process movement, can move diagonal 
void process_input(Game *pGame,SDL_Event *pEvent){
    const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_ESCAPE])
        {
            game_running = FALSE;
        }
            if (state[SDL_SCANCODE_UP])
        {
            //function to move up
        }
        if (state[SDL_SCANCODE_DOWN])
        {
            //function to move down
        }
        if (state[SDL_SCANCODE_LEFT])
        {
            //function to move left
        }
        if (state[SDL_SCANCODE_RIGHT])
        {
            //function to move
        }

}

//function to run the game with events linked to the main struct
void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    while(!close_requested){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT) close_requested = 1;
            else process_input(pGame,&event);
        }
        //updateChracter(pGame->pCharacter);

        SDL_SetRenderDrawColor(pGame->pRenderer,0,0,0,255);
        SDL_RenderClear(pGame->pRenderer);
        SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
        //drawCharacter(pGame->pCharacter);
        SDL_RenderPresent(pGame->pRenderer);
        SDL_Delay(1000/60-15);
    }
}

void close(Game *pGame){
    //if(pGame->pCharacter) destroyCharacter(pGame->pCharacter);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}