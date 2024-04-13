#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/game.h"
#include "../include/constants.h"
#include "../include/character.h"

enum GameState {MENU, ONGOING};
typedef enum GameState GameState;       //decides the state of the window 
struct ctx{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Character *pCharacter;
    SDL_Texture *background;
    SDL_Rect background_rect;
    GameState state;            //include the state in the main struct
};

typedef struct ctx CTX;

int main(int argc, char *argv[])
{
    // Initialize SDL and create a window and renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("MonkeyShooter",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load the background image
    SDL_Texture *background = IMG_LoadTexture(renderer, "resources/PrototypeMap.MS2.png");

    // Load the menu image
    SDL_Texture *menuTexture = IMG_LoadTexture(renderer, "resources/mMenu.png");

    // Create a new character
    Character *pCharacter = createCharacter(renderer);

    // Set the position and size of the background image
    SDL_Rect background_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // Set the position and size of the menu image
    SDL_Rect menu_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // Initialize game state so that it starts on menu
    GameState gameState = MENU;     

    // Main game loop
    int running = TRUE;
    SDL_Event event;
    const Uint8 *state;
    while (running) {
        state = SDL_GetKeyboardState(NULL);

        // Switch loop to handle the states 
        switch (gameState) {
            case MENU:
            
            SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect); // Render menu image if state is in menu

            // Check for Space key press to go to game 
            if (state[SDL_SCANCODE_SPACE]) {
                gameState = ONGOING;
            }
            break;
            case ONGOING:   //code for ingame inputs
                // Update character position based on user input
                pCharacter->moving_left = state[SDL_SCANCODE_A] > 0;
                pCharacter->moving_right = state[SDL_SCANCODE_D] > 0;
                pCharacter->moving_up = state[SDL_SCANCODE_W] > 0;
                pCharacter->moving_down = state[SDL_SCANCODE_S] > 0;

                if (state[SDL_SCANCODE_A]) {
                    turnLeft(pCharacter);
                }

                if (state[SDL_SCANCODE_D] && pCharacter->dest.x + pCharacter->dest.w < WINDOW_WIDTH) {
                    turnRight(pCharacter);
                }

                if (state[SDL_SCANCODE_W] && pCharacter->dest.y > 0) {
                    turnUpp(pCharacter);
                }

                if (state[SDL_SCANCODE_S] && pCharacter->dest.y + pCharacter->dest.h < WINDOW_HEIGHT) {
                    turnDown(pCharacter);
                }

                break;
        }

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Check the game state
        if (gameState == MENU) {
            // Render the menu image
            SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);
        }
        else if (gameState == ONGOING) {
            // Draw the background image on the screen
            SDL_RenderCopy(renderer, background, NULL, &background_rect);

            // Draw the character on the screen
            SDL_RenderCopyEx(renderer, pCharacter->tex, &pCharacter->source, &pCharacter->dest, 0, NULL, SDL_FLIP_NONE);
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Check for events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = FALSE;
            }
        }
    }

    // Clean up resources
    SDL_DestroyTexture(pCharacter->tex);
    SDL_DestroyTexture(background);
     SDL_DestroyTexture(menuTexture); // Destroy menu texture
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    destroyCharacter(pCharacter);

    return 0;
}