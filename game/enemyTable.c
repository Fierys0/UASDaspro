#include "entity.h"
#ifdef __unix__
  #include <ncurses.h>
#else
  #include <curses.h>
#endif

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
        slimeSprite,
        {100, COLOR_BLUE, COLOR_BLACK}
    };

    enemies[1] = (struct entityData){
        "Goblin", 50, 50,
        weapon[2],
        2, 3, 10, 24, 100,
        1.2f, 1.2f,
        {0,0,0,0},
        goblinSprite,
        {101, COLOR_GREEN, COLOR_BLACK}
    };
}

// const int entityCount = sizeof(enemies) / sizeof(enemies[0]);
