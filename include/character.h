#ifndef character_h
#define character_h

#define CHARACTER_HEIGHT 46
#define CHARACTER_WIDTH 46
#define MAX_HEALTH 4

typedef struct character Character;

Character *createCharacter(SDL_Renderer *renderer);
void decreaseHealth(Character *pCharacter);
int isCharacterAlive(Character *pCharacter);
void turnLeft(Character *pCharacter);
void turnRight(Character *pCharacter);
void turnUp(Character *pCharacter);
void turnDown(Character *pCharacter);
void updateCharacterAnimation(Character *pCharacter, Uint32 deltaTime);
void renderCharacter(Character *pCharacter, SDL_Renderer *renderer);
void destroyCharacter(Character *pCharacter);

#endif
