#include "game/core.c"
#include "UI/UI.c"

int main()
{
  initPlayer();
  initEnemies();
  mainUI();
  return 0;
}
