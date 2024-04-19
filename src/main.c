#include "../include/game.h"
#include "../include/character.h"
#include "../include/world.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define NR_OF_MENUTEXTURES 2



enum GameState {MENU, ONGOING};
typedef enum GameState GameState; 

enum menuState{MAIN, SETTINGS, CONFIGURE, INGAME};
typedef enum menuState MenuState; 

//main struct for game
struct menuTextures{
    SDL_Texture *SDLmTex[NR_OF_MENUTEXTURES];
    char MenuTextureFiles[NR_OF_MENUTEXTURES][60];
}; typedef struct menuTextures MenuTextures;
struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Character *pCharacter;
    SDL_Texture *background;
    MenuTextures *menuTextures;
    SDL_Rect background_rect;
    SDL_Rect menu_rect;
    GameState state;
    MenuState menuState;
}; typedef struct game Game;

int intializeWindow(Game *pGame); //removed renderer argument
void process_input(Game *pGame,SDL_Event *pEvent);
void run(Game *pGame);
void close(Game *pGame);


int main(int argv, char** args){
    Game g={0};
    if (!intializeWindow(&g)) return TRUE;      // if initializeWindow doesn't work end the program
    run(&g);            //or run and then close it after quitting
    close(&g);

    return 0;
}

//start the program and call needed from main struct
int intializeWindow(Game *pGame) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return FALSE;
    }
    
    pGame->menuTextures = malloc(sizeof(MenuTextures));
    
    if (pGame->menuTextures == NULL) {
        printf("Error allocating memory for menu textures: %s\n", SDL_GetError());
        return false; // Return false if memory allocation fails
    }

    // Initialize the SDLmTex pointers to NULL
    for (int i = 0; i < NR_OF_MENUTEXTURES; i++) {
        pGame->menuTextures->SDLmTex[i] = NULL;
    }
    
    strcpy(pGame->menuTextures->MenuTextureFiles[0], "resources/mMenu.png");
    strcpy(pGame->menuTextures->MenuTextureFiles[1], "resources/IPconfigure.png");

    pGame->pWindow = SDL_CreateWindow(
        "MonkeyShooter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!pGame->pWindow) {
        printf("Error creating window: %s\n", SDL_GetError());
        close(pGame);
        return FALSE;
    }

    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pGame->pRenderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return FALSE;
    }

    // Load the background image with error if it doens't work
    pGame->background = IMG_LoadTexture(pGame->pRenderer, "resources/PrototypeMap.MS2.png");
    if (!pGame->background) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return FALSE;
    }

    // Load the menu image with error if it doens't work
    for(int i = 0; i < NR_OF_MENUTEXTURES; i++){
        pGame->menuTextures->SDLmTex[i] = IMG_LoadTexture(pGame->pRenderer, pGame->menuTextures->MenuTextureFiles[i]);
        if (!pGame->menuTextures->SDLmTex[i]) {
            printf("Error loading menu image: %s\n", IMG_GetError());
            return FALSE;
        }
    }
    pGame->pCharacter = createCharacter(pGame->pRenderer);

    if (!pGame->pCharacter) {
        printf("Error: %s\n", SDL_GetError());
        close(pGame);
        return FALSE;
    }

    // Set the position and size of the background image
    pGame->background_rect = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // Set the position and size of the menu image
    pGame->menu_rect = (SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    pGame->state = MENU;

    return TRUE;
}




//function to run the game with events linked to the main struct
void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    while(!close_requested){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT) close_requested = TRUE;
        }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    int mouseX, mouseY, button;
        
        switch(pGame->state){
            case MENU:
                
                
                    
                button = SDL_GetMouseState(&mouseX, &mouseY);
                    
                if(mouseX>270 && mouseX<550 && mouseY>303 && mouseY<345 &&(button && SDL_BUTTON_LMASK)) pGame->state = ONGOING;  
                else if(mouseX>270 && mouseX<550 && mouseY>400 && mouseY<443 &&(button && SDL_BUTTON_LMASK)) pGame->menuState = SETTINGS;
                else if(mouseX>288 && mouseX<533 && mouseY>497 && mouseY<541 &&(button && SDL_BUTTON_LMASK)) pGame->menuState = CONFIGURE;
                else if(mouseX>320&& mouseX<499 && mouseY>593 && mouseY<637 &&(button && SDL_BUTTON_LMASK)) close_requested = TRUE;



                
                
                //switch(pGame->mState){
                //case SETTINGS:;
                //case CONFIGURE:
                

               case ONGOING:
                // Update character position based on user input while calling collision function from world.c to check if valid

                if (state[SDL_SCANCODE_A]) {
                    turnLeft(pGame->pCharacter);
                    if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                        //if collision stop movement
                        turnRight(pGame->pCharacter);
                    }
                }
                if (state[SDL_SCANCODE_D]) {
                    turnRight(pGame->pCharacter);
                    if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                        //if collision stop movement
                        turnLeft(pGame->pCharacter);
                    }
                }
                if (state[SDL_SCANCODE_W]) {
                    turnUpp(pGame->pCharacter);
                    if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                        //if collision stop movement
                        turnDown(pGame->pCharacter);
                    }
                }
                if (state[SDL_SCANCODE_S]) {
                    turnDown(pGame->pCharacter);
                    if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                        //if collision stop movement
                        turnUpp(pGame->pCharacter);
                    }
                }
                if(state[SDL_SCANCODE_ESCAPE]){
                    pGame->state = MENU;
                    pGame->menuState = MAIN;
                }

                break;
        }


        // Clear the renderer
        SDL_RenderClear(pGame->pRenderer);

        // Check the game state
        if (pGame->state == MENU && pGame->menuState == MAIN) {
            // Render the menu image
            SDL_RenderCopy(pGame->pRenderer, pGame->menuTextures->SDLmTex[0], NULL, &pGame->menu_rect);
        }
        else if(pGame->state == MENU && pGame->menuState == SETTINGS){
            SDL_RenderCopy(pGame->pRenderer, pGame->menuTextures->SDLmTex[1], NULL, &pGame->menu_rect);
        }
        if (pGame->state == ONGOING) {
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
    if(pGame->background) SDL_DestroyTexture(pGame->background); // Free the background texture
    for (int i = 0; i < NR_OF_MENUTEXTURES; i++){
        if(pGame->menuTextures->SDLmTex[i]) SDL_DestroyTexture(pGame->menuTextures->SDLmTex[i]);
    }
     // Free the menu texture
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}