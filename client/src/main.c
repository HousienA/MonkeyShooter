#include "../../lib/include/game.h"
#include "../../lib/include/character.h"
#include "../../lib/include/world.h"
#include "../../lib/include/bullet.h"
#include "../../lib/include/netdata.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

#define NR_OF_MENUTEXTURES 2

enum GameState {MENU, ONGOING};
typedef enum GameState GameState; 

enum menuState{MAIN, SETTINGS, CONFIGURE, INGAME};
typedef enum menuState MenuState; 

//struct for joining players

//main struct for game
struct menuTextures{
    SDL_Texture *SDLmTex[NR_OF_MENUTEXTURES];
    char MenuTextureFiles[NR_OF_MENUTEXTURES][60];
}; typedef struct menuTextures MenuTextures;
struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Character *pCharacter; // main player
    Character *pPlayers[MAX_PLAYERS]; // array of players to join
    SDL_Texture *background;
    MenuTextures *menuTextures;
    SDL_Rect background_rect;
    SDL_Rect menu_rect;
    GameState state;
    MenuState menuState;
    Bullet *bullets[1000];
    int num_bullets;
    int num_players; // track the number of players in the game
    SDL_Rect viewport;
    UDPsocket pSocket;
   IPaddress serverAddress;
   UDPpacket *pPacket;
}; typedef struct game Game;

int intializeWindow(Game *pGame); //removed renderer argument
int intializeNet(Game *pGame);
void process_input(Game *pGame,SDL_Event *pEvent);
void run(Game *pGame);
void close(Game *pGame);
void renderHealthBar(Character *pCharacter, SDL_Renderer *renderer);
void initializeCharacters(Game *pGame);
void renderCharacters(Game *pGame);


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
    pGame->background = IMG_LoadTexture(pGame->pRenderer, "resources/map1_2.png");
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
    
    //set viewport to be used in all functions
    pGame->viewport.x = 0;
    pGame->viewport.y = 0;
    pGame->viewport.w = WINDOW_WIDTH;
    pGame->viewport.h = WINDOW_HEIGHT;

    return TRUE;
}

void initializeCharacters(Game *pGame){
    // Initialize main player character
    pGame->pCharacter = createCharacter(pGame->pRenderer);
    if(!pGame->pCharacter){
        printf("Error creating main player character.\n");
        return; // Exit if main player character creation fails
    }

    // Initialize additional player characters
    for(int i = 0; i < MAX_PLAYERS - 1; i++){
        pGame->pPlayers[i] = createCharacter(pGame->pRenderer);
        if(!pGame->pPlayers[i]){
            printf("Error creating player %d.\n", i + 1);
            close(pGame);
            // Optionally handle the error or continue
        }
    }
    
    // Set the number of players (including the main player)
    pGame->num_players = 1;
}

void renderCharacters(Game *pGame){
    for(int i = 0; i < pGame->num_players; i++){
        Character *character = pGame->pPlayers[i];
        SDL_Rect characterDest = {
            character->dest.x - pGame->viewport.x,
            character->dest.y = pGame->viewport.y,
            character->dest.w,
            character->dest.h
        };
        SDL_RenderCopyEx(pGame->pRenderer, character->tex, &character->source, &characterDest, 0, NULL, SDL_FLIP_NONE);
    }
}

int intializeNet(Game *pGame){
   if(SDLNet_Init()){
       printf("Error: %s\n", SDLNet_GetError());
       SDL_Quit();
       return FALSE;
   }

   if (!(pGame->pSocket = SDLNet_UDP_Open(0)))//0 means not a server
   {
       printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
       return 0;
   }
   if (SDLNet_ResolveHost(&(pGame->serverAddress), "127.0.0.1", 2000))
   {
       printf("SDLNet_ResolveHost(127.0.0.1 2000): %s\n", SDLNet_GetError());
       return 0;
   }
   if (!(pGame->pPacket = SDLNet_AllocPacket(512)))
   {
       printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
       return 0;
   }
   pGame->pPacket->address.host = pGame->serverAddress.host;
   pGame->pPacket->address.port = pGame->serverAddress.port;
}




//function to run the game with events linked to the main struct
void handle_input(Game *pGame) {
    ClientData cData;
    static Uint32 lastShootTime = 0; // Variable to store the time of the last shot
    Uint32 currentTime = SDL_GetTicks(); // Get the current time in milliseconds
    int close_requested = FALSE;	
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    int mouseX, mouseY, button;
    int x,y;
    static int mouseClick = 0;

    switch(pGame->state){
        case MENU:
            button = SDL_GetMouseState(&mouseX, &mouseY);

            if(mouseX>270 && mouseX<550 && mouseY>303 && mouseY<345 &&(button && SDL_BUTTON_LMASK)) pGame->state = ONGOING;  
            else if(mouseX>270 && mouseX<550 && mouseY>400 && mouseY<443 &&(button && SDL_BUTTON_LMASK)) pGame->menuState = SETTINGS;
            else if(mouseX>288 && mouseX<533 && mouseY>497 && mouseY<541 &&(button && SDL_BUTTON_LMASK)) pGame->menuState = CONFIGURE;
            else if(mouseX>320&& mouseX<499 && mouseY>593 && mouseY<637 &&(button && SDL_BUTTON_LMASK)) close(pGame); // Exit the game

            break;

        case ONGOING:
            if (state[SDL_SCANCODE_A]) {
                turnLeft(pGame->pCharacter);
                if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnRight(pGame->pCharacter);
                }
            }
            if (state[SDL_SCANCODE_D]) {
                turnRight(pGame->pCharacter);
                if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnLeft(pGame->pCharacter);
                }
            }
            if (state[SDL_SCANCODE_W]) {
                turnUp(pGame->pCharacter);
                if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnDown(pGame->pCharacter);
                }
            }
            if (state[SDL_SCANCODE_S]) {
                turnDown(pGame->pCharacter);
                if (checkCollision(pGame->pCharacter, walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnUp(pGame->pCharacter);
                }
            }
            if (SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK && !mouseClick && currentTime - lastShootTime >= 1000) {
                // Shoot a bullet with location in respect to viewport 
                float bulletStartX = pGame->pCharacter->dest.x - pGame->viewport.x + pGame->pCharacter->dest.w / 2;
                float bulletStartY = pGame->pCharacter->dest.y - pGame->viewport.y + pGame->pCharacter->dest.h / 2;
                pGame->bullets[pGame->num_bullets] = createBullet(pGame->pRenderer, bulletStartX, bulletStartY);
                if (pGame->bullets[pGame->num_bullets]) {
                    // Calculate direction vector (normalized)
                    float dx = x - bulletStartX;
                    float dy = y - bulletStartY;
                    float mag = sqrtf(dx * dx + dy * dy);
                    pGame->bullets[pGame->num_bullets]->dx = dx / mag * BULLET_SPEED;
                    pGame->bullets[pGame->num_bullets]->dy = dy / mag * BULLET_SPEED;
                    pGame->num_bullets++;
                }                
                lastShootTime = currentTime;
            } else if (!(SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK)) { // If the button is not pressed, reset the flag
                mouseClick = 0;
            };

            break;
    }
     if(state[SDL_SCANCODE_ESCAPE]){
                pGame->state = MENU;
                pGame->menuState = MAIN;}
    
}

void run(Game *pGame) {
    int close_requested = 0;
    SDL_Event event;

    //size of view in the window, aka zoomed camera on character
    //SDL_Rect viewport = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; //viewport size to match window size

    //Initialize players
    initializeCharacters(pGame);

    while (!close_requested) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) close_requested = TRUE;
        }

        handle_input(pGame);

        //set viewport position to follow the player (player in the middle of screen)
        pGame->viewport.x = pGame->pCharacter->dest.x - (pGame->viewport.w / 2);
        pGame->viewport.y = pGame->pCharacter->dest.y - (pGame->viewport.h / 2);

        //if player is near edge of map move the player and keep the viewport in bounds
        if (pGame->viewport.x < 0) pGame->viewport.x = 0;
        if (pGame->viewport.y < 0) pGame->viewport.y = 0;
        if (pGame->viewport.x + pGame->viewport.w > MAP_WIDTH) pGame->viewport.x = MAP_WIDTH - pGame->viewport.w;
        if (pGame->viewport.y + pGame->viewport.h > MAP_HEIGHT) pGame->viewport.y = MAP_HEIGHT - pGame->viewport.h;

        // Render the game
        SDL_RenderClear(pGame->pRenderer);

        if (pGame->state == MENU && pGame->menuState == MAIN) {
            SDL_RenderCopy(pGame->pRenderer, pGame->menuTextures->SDLmTex[0], NULL, &pGame->menu_rect);
        }
        else if (pGame->state == MENU && pGame->menuState == SETTINGS) {
            SDL_RenderCopy(pGame->pRenderer, pGame->menuTextures->SDLmTex[1], NULL, &pGame->menu_rect);
        }
        if (pGame->state == ONGOING) {
            //Render players
            renderCharacters(pGame);

            // Render only the portion of the map that falls within the viewport
            SDL_Rect sourceRect = {pGame->viewport.x, pGame->viewport.y, pGame->viewport.w, pGame->viewport.h};
            SDL_RenderCopy(pGame->pRenderer, pGame->background, &sourceRect, NULL);

            // Draw the character on the screen within the viewport
            SDL_Rect characterDest = {
                pGame->pCharacter->dest.x - pGame->viewport.x,
                pGame->pCharacter->dest.y - pGame->viewport.y,
                pGame->pCharacter->dest.w,
                pGame->pCharacter->dest.h
            };
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pCharacter->tex, &pGame->pCharacter->source, &characterDest, 0, NULL, SDL_FLIP_NONE);

            for (int i = 0; i < pGame->num_bullets; i++) {
                moveBullet(pGame->bullets[i]);
                drawBullet(pGame->bullets[i], pGame->pRenderer);

               if (checkCollisionBulletCharacter(pGame->bullets[i], pGame->pCharacter)) {
                    pGame->pCharacter->health--;
                    destroyBullet(pGame->bullets[i]);
                    pGame->bullets[i] = NULL;
                    pGame->num_bullets--;
                }
            }
            renderHealthBar(pGame->pCharacter, pGame->pRenderer);
               // Check if character is dead
            if (pGame->pCharacter->health <= 0) {
                // Character is dead, reset the game
                pGame->state = MENU;
                pGame->menuState = MAIN;
                pGame->pCharacter->health = 4; // Reset character health
            }
        }

        SDL_RenderPresent(pGame->pRenderer);
        SDL_Delay(1000 / 60 - 15);
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

void renderHealthBar(Character *pCharacter, SDL_Renderer *renderer)
{
    SDL_Rect healthBar = {20, 20, 100, 20}; // Health bar position and size
    SDL_Rect remainingHealth = {20, 20, pCharacter->health * 25, 20}; // Health bar remaining size
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
    SDL_RenderFillRect(renderer, &healthBar); // Draw the background of health bar
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
    SDL_RenderFillRect(renderer, &remainingHealth); // Draw the remaining health
}