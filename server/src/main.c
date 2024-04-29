#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "../../lib/include/netdata.h"
#include "../../lib/include/character.h"
#include "../../lib/include/bullet.h"

#define NR_OF_MENUTEXTURES 2

enum GameState {MENU, ONGOING};
typedef enum GameState GameState; 

enum menuState{MAIN, SETTINGS, CONFIGURE, INGAME};
typedef enum menuState MenuState;
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
    int num_bullets, num_players, playerNumber, slotsTaken[4]; // track the number of players in the game
    SDL_Rect viewport;
    ServerData sData;
    UDPsocket pSocket;
    IPaddress serverAddress[MAX_PLAYERS];
    UDPpacket *pPacket;
}; typedef struct game Game;

int initiate(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void handleInput(Game *pGame,SDL_Event *pEvent);
void add(IPaddress address, IPaddress clients[],int *pNrOfClients);
void sendGameData(Game *pGame);
void executeCommand(Game *pGame,ClientData cData);
void setUpGame(Game *pGame);

int main(int argv, char** args){
    Game g={0};
    if(!initiate(&g)) return 1;
    run(&g);
    close(&g);

    return 0;
}

int initiate(Game *pGame){
    srand(time(NULL));
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }
    if(TTF_Init()!=0){
        printf("Error: %s\n",TTF_GetError());
        SDL_Quit();
        return 0;
    }
    if (SDLNet_Init())
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
		return 0;
	}

    pGame->pWindow = SDL_CreateWindow("Monkey Server",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    if(!pGame->pWindow){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }
    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!pGame->pRenderer){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;    
    }


    if (!(pGame->pSocket = SDLNet_UDP_Open(2000)))
	{
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		close(pGame);
        return 0;
	}
	if (!(pGame->pPacket = SDLNet_AllocPacket(512)))
	{
		printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		close(pGame);
        return 0;
	}

    for(int i=0;i<MAX_PLAYERS;i++) pGame->pPlayers[i] = createCharacter(pGame->pRenderer);
    
    pGame->num_players = MAX_MONKEYS;
    
    for(int i=0;i<MAX_MONKEYS;i++){
        if(!pGame->pPlayers[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }
   
    pGame->state = MENU;
    pGame->num_players = 0;

    
    return 1;
}

void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    ClientData cData;

    while(!close_requested){
        switch (pGame->state)
        {
            case ONGOING:
                sendGameData(pGame);
                
                while(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    memcpy(&cData, pGame->pPacket->data, sizeof(ClientData));
                    executeCommand(pGame,cData);
                }
                
                if(SDL_PollEvent(&event)) if(event.type==SDL_QUIT) close_requested = 1;
                
                for(int i=0;i<MAX_PLAYERS;i++)
                    pGame->pPlayers[cData.playerNumber]->health = cData.monkey.health;
                    pGame->pPlayers[cData.playerNumber]->dest.x = cData.monkey.vx;
                    pGame->pPlayers[cData.playerNumber]->dest.y = cData.monkey.vy;
                
                for(int i=0;i<MAX_PLAYERS;i++)
                    renderCharacter(pGame->pPlayers[i], pGame->pRenderer);
                
                SDL_RenderPresent(pGame->pRenderer);
                
                break;
            /*case :
                
                sendGameData(pGame);
                if(pGame->num_players==MAX_PLAYERS) pGame->num_players = 0;*/
            case MENU:
                printf("Waiting for players\n");
                if(SDL_PollEvent(&event) && event.type==SDL_QUIT) close_requested = 1;
                if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    add(pGame->pPacket->address,pGame->playerNumber,&(pGame->num_players));
                    if(pGame->num_players==MAX_MONKEYS) setUpGame(pGame);
                }
                break;
        }
        //SDL_Delay(1000/60-15);//might work when you run on different processors
    }
}

void setUpGame(Game *pGame){
    pGame->num_players=MAX_PLAYERS;
    pGame->state = ONGOING;
}

void sendGameData(Game *pGame){
    ServerData sData;
    sData.gState = pGame->state;
    for(int i=0;i<MAX_PLAYERS;i++){
        sData.slotsTaken[i] = pGame->slotsTaken[i];
        sData.monkeys[i].x = pGame->pPlayers[i]->dest.x;
        sData.monkeys[i].y = pGame->pPlayers[i]->dest.y;
    }

    for(int i=0;i<MAX_PLAYERS;i++){
        memcpy(pGame->pPacket->data, &(pGame->sData), sizeof(ServerData));
		pGame->pPacket->len = sizeof(ServerData);
        pGame->pPacket->address = pGame->serverAddress[i];
		SDLNet_UDP_Send(pGame->pSocket,-1,pGame->pPacket);
    }
}

void add(IPaddress address, IPaddress clients[],int *pNrOfClients){
	for(int i=0;i<*pNrOfClients;i++) if(address.host==clients[i].host &&address.port==clients[i].port) return;
	clients[*pNrOfClients] = address;
	(*pNrOfClients)++;
}

void executeCommand(Game *pGame,ClientData cData){
    switch (cData.command)
    {
        case UP:
            turnUp(pGame->pPlayers[cData.playerNumber]);
            break;
        case DOWN:
            turnDown(pGame->pPlayers[cData.playerNumber]);
            break;
        case LEFT:
            turnLeft(pGame->pPlayers[cData.playerNumber]);
            break;
        case RIGHT:
            turnRight(pGame->pPlayers[cData.playerNumber]);
            break;
        case FIRE:
            //createBullet(pGame->pPlayers[cData.playerNumber],cData.monkey.);
            break;
    }
}

void close(Game *pGame){
    for(int i=0;i<MAX_PLAYERS;i++) if(pGame->pPlayers[i]) destroyCharacter(pGame->pPlayers[i]);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);

    if(pGame->pPacket) SDLNet_FreePacket(pGame->pPacket);
	if(pGame->pSocket) SDLNet_UDP_Close(pGame->pSocket);

    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}

