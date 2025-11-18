#include "entity.h"

struct entityData enemies[3];  // Declare but don’t initialize weapons yet
extern const char* goblinSprite;
extern const char* slimeSprite;

void initEnemies(void) {
    enemies[0] = (struct entityData){
        "Slime", 20, 20,
        weapon[0],
        1, 1, 4, 20, 50,
        1.0f, 1.0f,
        {0,0,0,0},
        slimeSprite
    };

    enemies[1] = (struct entityData){
        "Goblin", 50, 50,
        weapon[2],
        2, 3, 10, 24, 100,
        1.2f, 1.2f,
        {0,0,0,0},
        goblinSprite
    };
}

// const int entityCount = sizeof(enemies) / sizeof(enemies[0]);
