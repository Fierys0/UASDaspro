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

bool isSecret = false;

int main(int argc, char *argv[])
{
  if (argc > 1 && strcmp(argv[1], "debug") == 0) isSecret = true;
  setlocale(LC_ALL, "");
  initscr();
  initPlayer();
  initEnemies();
  titleScreen();
  mainUI(isSecret);
  return 0;
}
