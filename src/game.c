#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/game.h"


int main(int argc, char *argv[]){

    // initialize SDL
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("MonkeyShooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, PLAYER_WIDTH, PLAYER_HEIGHT, SDL_WINDOW_OPENGL);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);

  //LOad image file
  SDL_Surface* image = IMG_Load("resources/character.png");
  SDL_Texture* image_texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);

  //Define where on the screen we want the character
  SDL_Rect texture_destination;
  texture_destination.x = 100;
  texture_destination.y = 50;
  texture_destination.w = 250;
  texture_destination.h = 250;



  int running = TRUE;
  SDL_Event event;
  while(running){
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            running = FALSE;
        }
    }

    //clear screen
    SDL_RenderClear(renderer);

    //draw
    SDL_RenderCopy(renderer, image_texture, NULL, &texture_destination);


    // show what was drawn
    SDL_RenderPresent(renderer);
  }

  //Release resources
  SDL_DestroyTexture(image_texture);
  IMG_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

