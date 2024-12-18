#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "../../lib/include/netdata.h"
#include "../../lib/include/character.h"
#include "../../lib/include/bullet.h"
#include "../../lib/include/text.h"
#include "../../lib/include/game.h"

#define NR_OF_MENUTEXTURES 2


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
    Bullet *bullets[MAX_BULLETS];
    int num_bullets, num_players, slotsTaken[4], fire; // track the number of players in the game
    TTF_Font *font;
    Text *pWaitingText, *pJoinedText;
    ServerData sData;
    
    UDPsocket pSocket;
    IPaddress serverAddress[MAX_PLAYERS];
    UDPpacket *pPacket;
    
}; typedef struct game Game;

int initiate(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void add(IPaddress address, IPaddress clients[],int *pNrOfClients);
void sendGameData(Game *pGame, ClientData cData);
void executeCommand(Game *pGame,ClientData cData);
void acceptClients(Game *pGame, ClientData cData);

 

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
    if (SDLNet_Init())
    {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    return 1;
    }

    pGame->font = TTF_OpenFont("../lib/resources/arial.ttf", 60);
    if (!pGame->font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
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
    pGame->background = IMG_LoadTexture(pGame->pRenderer, "../lib/resources/monkeyMap.png");
    if (!pGame->background) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return FALSE;
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

    for(int i=0;i<MAX_PLAYERS;i++) pGame->pPlayers[i] = createCharacter(pGame->pRenderer, i+1);
    
    pGame->num_players = MAX_MONKEYS;
    
    for(int i=0;i<MAX_MONKEYS;i++){
        if(!pGame->pPlayers[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }
    pGame->pWaitingText = createText(pGame->pRenderer, 255, 255, 255, pGame->font, "Waiting for players", 400, 400);
    pGame->pJoinedText = createText(pGame->pRenderer, 255, 255, 255, pGame->font, "All players joined", 400, 400);
   
    pGame->state = MENU;
    pGame->num_players = 0;
    return 1;
}

void acceptClients(Game *pGame, ClientData cData){
    while(pGame->num_players < MAX_PLAYERS) {
        if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1) {     //Receive packets
            //Add player
            add(pGame->pPacket->address, pGame->serverAddress, &(pGame->num_players));
            sendGameData(pGame, cData);
        } else {         
            break;      //No incoming packets, break out of the loop
        }
    }
}
void renderCharacters(Game *pGame){
    for(int i = 0; i < pGame->num_players; i++){
        Character *character = pGame->pPlayers[i];
        renderCharacter(character, pGame->pRenderer);       //loop through all players and render them
    }
}

void run(Game *pGame){
    int close_requested = 0;
    for(int i=0;i<MAX_PLAYERS;i++) pGame->slotsTaken[i] = 0;
    pGame->num_bullets = 0; 
    SDL_Event event;
    ClientData cData = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    while(!close_requested){
       
        switch (pGame->state)
        {
            case ONGOING:
                sendGameData(pGame,cData);
                SDL_RenderCopy(pGame->pRenderer, pGame->background, NULL, NULL);
                renderCharacters(pGame);
                if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    
                    
                    if(cData.command[5]==FIRE) printf("fire=on");
                
                    if(cData.command[5]==FIRE) printf("fire=on");
                    memcpy(&cData, pGame->pPacket->data, sizeof(ClientData));
                    executeCommand(pGame,cData);
                    sendGameData(pGame, cData);
                    memset(&cData, 0, sizeof(cData));
                    
                    
                }
                for (int i = 0; i < pGame->num_bullets; i++){
                    if(pGame->bullets[i] == NULL) continue;
                    drawBullet(pGame->bullets[i], pGame->pRenderer);
                    moveBullet(pGame->bullets[i]);
                    moveBullet(pGame->bullets[i]);
                        
                    moveBullet(pGame->bullets[i]);        
                        
                }

                if(SDL_PollEvent(&event)) {
                    if(event.type==SDL_QUIT) {
                        close_requested = 1;
                        break; 
                    }
                }
                
                SDL_RenderPresent(pGame->pRenderer);
                break;
            case MENU:
                acceptClients(pGame, cData);
                drawText(pGame->pWaitingText);
                SDL_RenderPresent(pGame->pRenderer);
                sendGameData(pGame, cData);
                
                if(SDL_PollEvent(&event) && event.type==SDL_QUIT) {
                    close_requested = 1;
                    break; 
                }
                if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    add(pGame->pPacket->address,pGame->serverAddress,&(pGame->num_players));
                    
                    printf("numberofplayers%d\n",pGame->num_players);
                    sendGameData(pGame, cData);
                    pGame->slotsTaken[pGame->num_players-1] = 1;
                    
                    if(pGame->num_players==MAX_MONKEYS){
                        pGame->state = ONGOING;
                        destroyText(pGame->pWaitingText);
                    }
                     
                }
                break;
        }
    }
}



void add(IPaddress address, IPaddress clients[],int *pNrOfClients){
	for(int i=0;i<*pNrOfClients;i++) if(address.host==clients[i].host &&address.port==clients[i].port) return;
	clients[*pNrOfClients] = address;
	(*pNrOfClients)++;
}

void executeCommand(Game *pGame,ClientData cData){
    if(cData.command[1]==UP&& cData.command[6]!=BLOCKED) turnUp(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[2]==DOWN&& cData.command[6]!=BLOCKED) turnDown(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[3]==LEFT&& cData.command[6]!=BLOCKED) turnLeft(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[4]==RIGHT&& cData.command[6]!=BLOCKED) turnRight(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[5]==FIRE){
        printf("%d player in fired\n", cData.playerNumber);
        pGame->bullets[pGame->num_bullets] = createBullet(pGame->pRenderer, cData.bulletStartX, cData.bulletStartY, cData.playerNumber);
        if (pGame->bullets[pGame->num_bullets]) {
            pGame->bullets[pGame->num_bullets]->dx = cData.bulletDx;
            pGame->bullets[pGame->num_bullets]->dy = cData.bulletDy;
            pGame->num_bullets++;
            pGame->fire = 1;
            return;
        }
    }
}

void sendGameData(Game *pGame,ClientData cData){
    ServerData sData;
    sData.gState = pGame->state;
    for(int i=0;i<MAX_MONKEYS;i++){
        sData.slotsTaken[i] = pGame->slotsTaken[i];
        characterSendData(pGame->pPlayers[i], &(sData.monkeys[i]));
        sData.whoShot = cData.playerNumber;
        
    }
    
    if(pGame->num_bullets>0){
    sData.bulletDx = DxBullet(pGame->bullets[pGame->num_bullets-1]);
    sData.bulletDy = DyBullet(pGame->bullets[pGame->num_bullets-1]);
    sData.bulletStartX = xBullet(pGame->bullets[pGame->num_bullets-1]);
    sData.bulletStartY = yBullet(pGame->bullets[pGame->num_bullets-1]);
    
    sData.fire = pGame->fire;
    pGame->fire = 0;
    }
    sData.numberOfPlayers = pGame->num_players;
    sData.numberOfBullets = pGame->num_bullets;
    
    memcpy(pGame->pPacket->data, &(sData), sizeof(ServerData));
    pGame->pPacket->len = sizeof(ServerData);
    for(int i=0;i<pGame->num_players;i++){
        pGame->pPacket->address = pGame->serverAddress[i];
        if(0==SDLNet_UDP_Send(pGame->pSocket,-1,pGame->pPacket)) printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
    }
}

void close(Game *pGame){
    for(int i=0;i<MAX_PLAYERS;i++) if(pGame->pPlayers[i]) destroyCharacter(pGame->pPlayers[i]);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    
    for (int i = 0; i < pGame->num_bullets; i++) {
        if (pGame->bullets[i]) {
            destroyBullet(pGame->bullets[i]);
            pGame->bullets[i] = NULL;
        }
    }

    if(pGame->pPacket) SDLNet_FreePacket(pGame->pPacket);
	if(pGame->pSocket) SDLNet_UDP_Close(pGame->pSocket);

    destroyText(pGame->pWaitingText);
    TTF_CloseFont(pGame->font);
    TTF_Quit();
    SDLNet_Quit();    
    SDL_Quit();
}

