#include "game/core.c"
#include "UI/UI.c"
#include "UI/titleScreenUI.c"
#include <locale.h>
#ifdef __unix__
  #include <ncurses.h>
#else
  #include <curses.h>
  #include <windows.h>
#endif

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
