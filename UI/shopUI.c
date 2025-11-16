WINDOW * shopHud;

extern WINDOW *debugHud, *playerhud, *mainScreen, *commandHud, *textHud, *debugMessageHud;
extern void center_box(WINDOW *parent, WINDOW *child, int y_offset, int styley, int stylex);
extern void clearCommandHud(); extern void drawMainScreen();
extern int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx);
extern void printCenteredSprite(WINDOW *win, int start_y, const char **lines, int lineCount);


void startShop()
{
  shopHud = newwin(9, 29, 0 ,0);
  center_box(mainScreen, shopHud, 10, 0, 0);
  mvwprintw(shopHud, 1, 1, shopSprite);
  box(shopHud, 0, 0);
  wrefresh(shopHud);

  char *shopChoices[] = {"Weapon", "Armor", "<< Back", NULL};
  while (1)
  {
    int choices = usrInputChoices(shopChoices, commandHud, 1, 1);
    if (choices == 2) break;
  }

  clearCommandHud();
  delwin(shopHud);
  drawMainScreen();
}
