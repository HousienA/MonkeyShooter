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
    Character *pPlayers[MAX_PLAYERS];  
    SDL_Texture *background;
    MenuTextures *menuTextures;
    SDL_Rect background_rect;
    SDL_Rect menu_rect;
    GameState state;
    MenuState menuState;
    Bullet *bullets[1000];
    int num_bullets, num_players, playerNumber, slotsTaken[MAX_PLAYERS]; // track the number of players in the game
    SDL_Rect viewport;

    UDPsocket pSocket;
    IPaddress serverAddress;
    UDPpacket *pPacket;
}; typedef struct game Game;

int intializeWindow(Game *pGame); //removed renderer argument
int initializeNetwork(Game *pGame);
void process_input(Game *pGame,SDL_Event *pEvent);
void run(Game *pGame);
void close(Game *pGame);
void renderHealthBar(Character *pPlayers[MAX_PLAYERS], SDL_Renderer *renderer, int playerNumber);
void handleBulletCreation(Game *pGame, int x, int y);
void handle_settings(Game *pGame, const Uint8 *state);
void initializeCharacters(Game *pGame);
void renderCharacters(Game *pGame);
void sendData(Game *pGame, ClientData *cData);
void updateWithServerData(Game *pGame);
void updateMonkeysWithRecievedData(Character *pPlayers, MonkeyData *monkeys);

int main(int argv, char** args){
    Game g={0};
    if (!intializeWindow(&g)) return TRUE; 
    if(!initializeNetwork(&g))return TRUE;     // if initializeWindow doesn't work end the program
    run(&g);            //or run and then close it after quitting
    close(&g);

    return 0;
}

int initializeNetwork(Game *pGame){
    if (SDLNet_Init())
	{
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
		return 0;
	}
     if (!(pGame->pSocket = SDLNet_UDP_Open(0)))//0 means not a server
	{
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		return 0;
	}
	// Resolve the server address
    if (SDLNet_ResolveHost(&(pGame->serverAddress), "127.0.0.1", 2000) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pGame->pSocket); // Close the socket
        SDLNet_Quit(); // Cleanup SDLNet
        return 0;
    }

    // Allocate memory for the UDP packet
    pGame->pPacket = SDLNet_AllocPacket(16064);
    if (!pGame->pPacket) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pGame->pSocket); // Close the socket
        SDLNet_Quit(); // Cleanup SDLNet
        return 0;
    }
    pGame->pPacket->address.host = pGame->serverAddress.host;
    pGame->pPacket->address.port = pGame->serverAddress.port;
    return 1;
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
    
    strcpy(pGame->menuTextures->MenuTextureFiles[0], "../lib/resources/mMenu.png");
    strcpy(pGame->menuTextures->MenuTextureFiles[1], "../lib/resources/IPconfigure.png");

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
    pGame->background = IMG_LoadTexture(pGame->pRenderer, "../lib/resources/map1_2.png");
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
    pGame->num_players = 4;
    // Initialize additional player characters
    for(int i = 0; i < pGame->num_players; i++){
        pGame->pPlayers[i] = createCharacter(pGame->pRenderer);
        if(!pGame->pPlayers[i]){
            printf("Error creating player %d.\n", i + 1);
            close(pGame);
            // Optionally handle the error or continue
        }
    }
    
    // Set the number of players (including the main player)
    
}

void renderCharacters(Game *pGame){
    for(int i = 0; i < pGame->num_players; i++){
        Character *character = pGame->pPlayers[i];
        SDL_Rect characterDest = {
            pGame->pPlayers[i]->dest.x - pGame->viewport.x,
            pGame->pPlayers[i]->dest.y - pGame->viewport.y,
            pGame->pPlayers[i]->dest.w,
            pGame->pPlayers[i]->dest.h
        };
        printf("Player: x: %d, y: %d\n", characterDest.x, characterDest.y);
        SDL_RenderCopyEx(pGame->pRenderer, character->tex, &character->source, &characterDest, 0, NULL, SDL_FLIP_NONE);
    }
}

//function to handle the creation of bullets
void handleBulletCreation(Game *pGame, int x, int y) {
    float bulletStartX = pGame->pPlayers[pGame->playerNumber]->dest.x - pGame->viewport.x + pGame->pPlayers[pGame->playerNumber]->dest.w / 2;
    float bulletStartY = pGame->pPlayers[pGame->playerNumber]->dest.y - pGame->viewport.y + pGame->pPlayers[pGame->playerNumber]->dest.h / 2;
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
}

//function to handle the settings menu
void handle_settings(Game *pGame, const Uint8 *state) {
    if (state[SDL_SCANCODE_1] && pGame->slotsTaken[0] != 1) {
        pGame->playerNumber = 0; // Player 1
        pGame->slotsTaken[0] = 1;
    } else if (state[SDL_SCANCODE_2] && pGame->slotsTaken[1] != 1) {
        pGame->playerNumber = 1; // Player 2
        pGame->slotsTaken[1] = 1;
    } else if (state[SDL_SCANCODE_3] && pGame->slotsTaken[2] != 1) {
        pGame->playerNumber = 2; // Player 3
        pGame->slotsTaken[2] = 1;
    } else if (state[SDL_SCANCODE_4] && pGame->slotsTaken[3] != 1) {
        pGame->playerNumber = 3; // Player 4
        pGame->slotsTaken[3] = 1;
    }
    printf("Player number: %d\n", pGame->playerNumber);
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

            if(mouseX>270 && mouseX<550 && mouseY>303 && mouseY<345 &&(button && SDL_BUTTON_LMASK)){  pGame->state = ONGOING;  
                    cData.command=READY;
                    cData.playerNumber=-1;
                    memcpy(pGame->pPacket->data, &cData, sizeof(ClientData));
		            pGame->pPacket->len = sizeof(ClientData);
                    SDLNet_UDP_Send(pGame->pSocket, -1,pGame->pPacket);
            }
            else if(mouseX>270 && mouseX<550 && mouseY>400 && mouseY<443 &&(button && SDL_BUTTON_LMASK)) pGame->menuState = SETTINGS;
            else if(mouseX>288 && mouseX<533 && mouseY>497 && mouseY<541 &&(button && SDL_BUTTON_LMASK)) pGame->menuState = CONFIGURE;
            else if(mouseX>320&& mouseX<499 && mouseY>593 && mouseY<637 &&(button && SDL_BUTTON_LMASK)) close(pGame); // Exit the game
        
            switch (pGame->menuState) {
                case SETTINGS:
                    handle_settings(pGame, state);
                    break;
                default:
                    break;
            }

            break;

        case ONGOING:
            if (state[SDL_SCANCODE_A]) {
                turnLeft(pGame->pPlayers[ pGame->playerNumber]);
                cData.command = LEFT;
                if (checkCollision(pGame->pPlayers[pGame->playerNumber], walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnRight(pGame->pPlayers[pGame->playerNumber]);
                    cData.command = RIGHT;
                }
            }
            if (state[SDL_SCANCODE_D]) {
                turnRight(pGame->pPlayers[pGame->playerNumber]);
                cData.command = RIGHT;
                if (checkCollision(pGame->pPlayers[pGame->playerNumber], walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnLeft(pGame->pPlayers[pGame->playerNumber]);
                    cData.command = LEFT;
                }
            }
            if (state[SDL_SCANCODE_W]) {
                turnUp(pGame->pPlayers[pGame->playerNumber]);
                cData.command = UP;
                if (checkCollision(pGame->pPlayers[pGame->playerNumber], walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnDown(pGame->pPlayers[pGame->playerNumber]);
                    cData.command = DOWN;
                }
            }
            if (state[SDL_SCANCODE_S]) {
                turnDown(pGame->pPlayers[pGame->playerNumber]);
                cData.command = DOWN;
                if (checkCollision(pGame->pPlayers[pGame->playerNumber], walls, sizeof(walls) / sizeof(walls[0]))) {
                    turnUp(pGame->pPlayers[pGame->playerNumber]);
                    cData.command = UP;
                }
            }
            if (SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK && !mouseClick && currentTime - lastShootTime >= 1000) {
                // Shoot a bullet with location in respect to viewport 
                handleBulletCreation(pGame, x, y);
                cData.command = FIRE;              
                lastShootTime = currentTime;
            } else if (!(SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK)) { // If the button is not pressed, reset the flag
                mouseClick = 0;
            };
            sendData(pGame, &cData);
            break;
    }
     if(state[SDL_SCANCODE_ESCAPE]){
                pGame->state = MENU;
                pGame->menuState = MAIN;}
    
    //cData.monkey.x = pGame->pPlayers[pGame->playerNumber]->dest.x;
    //cData.monkey.y = pGame->pPlayers[pGame->playerNumber]->dest.y;
    /*for(int i = pGame->num_bullets-1; i < pGame->num_bullets; i++){
        cData.monkey.bData[i].x = pGame->bullets[i]->x;
        cData.monkey.bData[i].y = pGame->bullets[i]->y;
        cData.monkey.bData[i].dx = pGame->bullets[i]->dx;
        cData.monkey.bData[i].dy = pGame->bullets[i]->dy;
    }*/

    
    
}

void run(Game *pGame) {
    int close_requested = 0;
    SDL_Event event;

    //Initialize players
    initializeCharacters(pGame);

    while (!close_requested) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) close_requested = TRUE;
        }
        //updateWithServerData(pGame);
        
        handle_input(pGame);

        //set viewport position to follow the player (player in the middle of screen)
        pGame->viewport.x = pGame->pPlayers[pGame->playerNumber]->dest.x - (pGame->viewport.w / 2);
        pGame->viewport.y = pGame->pPlayers[pGame->playerNumber]->dest.y - (pGame->viewport.h / 2);

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
                pGame->pPlayers[pGame->playerNumber]->dest.x - pGame->viewport.x,
                pGame->pPlayers[pGame->playerNumber]->dest.y - pGame->viewport.y,
                pGame->pPlayers[pGame->playerNumber]->dest.w,
                pGame->pPlayers[pGame->playerNumber]->dest.h
            };
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayers[pGame->playerNumber]->tex, &pGame->pPlayers[pGame->playerNumber]->source, &characterDest, 0, NULL, SDL_FLIP_NONE);

            for (int i = 0; i < pGame->num_bullets; i++) {
                moveBullet(pGame->bullets[i]);
                drawBullet(pGame->bullets[i], pGame->pRenderer);

               if (checkCollisionBulletCharacter(pGame->bullets[i], pGame->pPlayers[pGame->playerNumber])) {
                    pGame->pPlayers[pGame->playerNumber]->health--;
                    destroyBullet(pGame->bullets[i]);
                    pGame->bullets[i] = NULL;
                    pGame->num_bullets--;
                }
            }
            renderHealthBar(pGame->pPlayers, pGame->pRenderer, pGame->playerNumber);
               // Check if character is dead
            if (pGame->pPlayers[pGame->playerNumber]->health <= 0) {
                // Character is dead, reset the game
                pGame->state = MENU;
                pGame->menuState = MAIN;
                pGame->pPlayers[pGame->playerNumber]->health = 4; // Reset character health
            }
        }

        SDL_RenderPresent(pGame->pRenderer);
        SDL_Delay(1000 / 60 - 15);
    }
}


void renderHealthBar(Character *pPlayers[MAX_PLAYERS], SDL_Renderer *renderer, int playerNumber)
{
    SDL_Rect healthBar = {20, 20, 100, 20}; // Health bar position and size
    SDL_Rect remainingHealth = {20, 20, pPlayers[playerNumber]->health * 25, 20}; // Health bar remaining size
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
    SDL_RenderFillRect(renderer, &healthBar); // Draw the background of health bar
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
    SDL_RenderFillRect(renderer, &remainingHealth); // Draw the remaining health
}
void sendData(Game *pGame, ClientData *cData){
    // Use the playerNumber that's selected in the settings menu
    cData->playerNumber = pGame->playerNumber;

    // Check if playerNumber is within the valid range
    if (cData->playerNumber >= 0 && cData->playerNumber < pGame->num_players) {
        cData->monkey.x = pGame->pPlayers[cData->playerNumber]->dest.x;
        cData->monkey.y = pGame->pPlayers[cData->playerNumber]->dest.y;
        cData->monkey.health = pGame->pPlayers[cData->playerNumber]->health;
        
        memcpy(pGame->pPacket->data, cData, sizeof(ClientData));
        SDLNet_UDP_Send(pGame->pSocket, -1, pGame->pPacket);
    }
}

/*
void updateWithServerData(Game *pGame){
    ServerData sData;
    memcpy(&sData, pGame->pPacket->data, sizeof(ServerData));
    
    pGame->state = sData.gState;
    pGame->num_bullets = sData.numberOfBullets;
    pGame->bullets[sData.numberOfBullets] = sData.bullets[sData.numberOfBullets];
    for(int i=0;i<MAX_PLAYERS;i++){
        pGame->slotsTaken[i] = sData.slotsTaken[i];
        updateMonkeysWithRecievedData(pGame->pPlayers[i],&(sData.monkeys[i]));
    }
}   
*/

void updateWithServerData(Game *pGame){
    if(SDLNet_UDP_Recv(pGame->pSocket, pGame->pPacket)){
        ServerData sData;
        memcpy(&sData, pGame->pPacket->data, sizeof(ServerData));
        pGame->state = sData.gState;
        for(int i=0;i<MAX_MONKEYS;i++){
            pGame->pPlayers[i]->dest.x = sData.monkeys[i].x;
            pGame->pPlayers[i]->dest.y = sData.monkeys[i].y;
            pGame->pPlayers[i]->source.x = sData.monkeys[i].vx;
            pGame->pPlayers[i]->source.y = sData.monkeys[i].vy;
            pGame->pPlayers[i]->health = sData.monkeys[i].health;
        }
    }
}

void close(Game *pGame){
    for(int i = 0; i < MAX_PLAYERS; i++){
         destroyCharacter(pGame->pPlayers[i]); 
    }
    for (int i = 0; i < pGame->num_bullets; i++) {
    destroyBullet(pGame->bullets[i]);
}
    
    if(pGame->background) SDL_DestroyTexture(pGame->background); // Free the background texture
    for (int i = 0; i < NR_OF_MENUTEXTURES; i++){
        if(pGame->menuTextures->SDLmTex[i]) SDL_DestroyTexture(pGame->menuTextures->SDLmTex[i]);
    }
    if(pGame->menuTextures) {
    free(pGame->menuTextures);
    pGame->menuTextures = NULL;
    }
     // Free the menu texture
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}