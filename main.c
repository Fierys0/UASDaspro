#include "game/core.c"
#include "UI/UI.c"
#include "UI/titleScreenUI.c"
#include <locale.h>
#include <ncurses.h>

int main()
{
  setlocale(LC_ALL, "");
  initscr();
  initPlayer();
  initEnemies();
  titleScreen();
  mainUI();
  return 0;
}
