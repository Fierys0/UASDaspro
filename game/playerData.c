#include "entity.h"

struct Player player;

void initPlayer()
{
    player = (struct Player){
    "Hero",
    100, 100,
    weapon[0],
    armor[0],
    1, 10, 0, 5, 5, 3, 0, 0,
    {0,0,0,0}

    };
}

