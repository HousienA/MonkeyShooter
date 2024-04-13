#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/game.h"
#include "../include/constants.h"
#include "../include/character.h"

int game_running = FALSE;

enum GameState {MENU, ONGOING};
typedef enum GameState GameState; 

//main struct for game
struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Character *pCharacter;
    SDL_Texture *background;
    SDL_Texture *menuTexture;
    SDL_Rect background_rect;
    SDL_Rect menu_rect;
    GameState state;
}; typedef struct game Game;

int intializeWindow(Game *pGame, SDL_Renderer *renderer);
void process_input(Game *pGame,SDL_Event *pEvent);
void run(Game *pGame);
void close(Game *pGame);


int main(int argv, char** args){
    Game g={0};
    if (!intializeWindow(&g, g.pRenderer)) return TRUE;      // if initializeWindow doesn't work end the program
    run(&g);            //or run and then close it after quitting
    close(&g);

    return 0;
}

//start the program and call needed from main struct
int intializeWindow(Game *pGame, SDL_Renderer *renderer){
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

        // Load the background image
        SDL_Texture *background = IMG_LoadTexture(renderer, "resources/PrototypeMap.MS2.png");

        // Load the menu image
        SDL_Texture *menuTexture = IMG_LoadTexture(renderer, "resources/mMenu.png");

        pGame->pCharacter = createCharacter(pGame->pRenderer);

        if (!pGame->pCharacter)
        {
            printf("Error: %s\n", SDL_GetError());
            close(pGame);
            return FALSE;
        }

        // Set the position and size of the background image
        pGame->background_rect = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

        // Set the position and size of the menu image
        pGame->menu_rect = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

        pGame->state = MENU; // osäker om denna ska finnas här

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
            turnUpp(pGame->pCharacter);
        }
        if (state[SDL_SCANCODE_DOWN])
        {
            turnDown(pGame->pCharacter);
        }
        if (state[SDL_SCANCODE_LEFT])
        {
            turnLeft(pGame->pCharacter);
        }
        if (state[SDL_SCANCODE_RIGHT])
        {
            turnRight(pGame->pCharacter);
        }

}

//function to run the game with events linked to the main struct
void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    while(!close_requested){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT) close_requested = TRUE;
            else process_input(pGame,&event);
        }

        switch(pGame->state){
            case MENU:
                SDL_RenderCopy(pGame->pRenderer, pGame->menuTexture, NULL, &pGame->menu_rect); // Render menu image if state is in menu
                break;
            case ONGOING:
                // Update character position based on user input
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                pGame->pCharacter->moving_left = state[SDL_SCANCODE_A] > 0;
                pGame->pCharacter->moving_right = state[SDL_SCANCODE_D] > 0;
                pGame->pCharacter->moving_up = state[SDL_SCANCODE_W] > 0;
                pGame->pCharacter->moving_down = state[SDL_SCANCODE_S] > 0;

                if (state[SDL_SCANCODE_A]) {
                    turnLeft(pGame->pCharacter);
                }
                if (state[SDL_SCANCODE_D] && pGame->pCharacter->dest.x + pGame->pCharacter->dest.w < WINDOW_WIDTH) {
                    turnRight(pGame->pCharacter);
                }
                if (state[SDL_SCANCODE_W] && pGame->pCharacter->dest.y > 0) {
                    turnUpp(pGame->pCharacter);
                }
                if (state[SDL_SCANCODE_S] && pGame->pCharacter->dest.y + pGame->pCharacter->dest.h < WINDOW_HEIGHT) {
                    turnDown(pGame->pCharacter);
                }
                break;
        }

        // Clear the renderer
        SDL_RenderClear(pGame->pRenderer);

        // Check the game state
        if (pGame->state == MENU) {
            // Render the menu image
            SDL_RenderCopy(pGame->pRenderer, pGame->menuTexture, NULL, &pGame->menu_rect);
        }
        else if (pGame->state == ONGOING) {
            // Draw the background image on the screen
            SDL_RenderCopy(pGame->pRenderer, pGame->background, NULL, &pGame->background_rect);
            // Draw the character on the screen
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pCharacter->tex, &pGame->pCharacter->source, &pGame->pCharacter->dest, 0, NULL, SDL_FLIP_NONE);
        }

        // Update the screen
        SDL_RenderPresent(pGame->pRenderer);
        // Delay to control frame rate
        SDL_Delay(1000/60-15);
    }
}

void close(Game *pGame){
    if(pGame->pCharacter) destroyCharacter(pGame->pCharacter);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}