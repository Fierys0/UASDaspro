
WINDOW * enemySprite, * enemyHealthHud;

extern WINDOW * mainScreen, * textHud, *commandHud;
extern bool isDebug;
extern void center_box(WINDOW *parent, WINDOW *child, int y_offset);
extern int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx);
extern void debugMenuInput(int usrInput);
extern WINDOW *debugMenu();
extern void draw_all();
extern void handle_resize(int sig);
extern int exitMenu();
extern void debugMenuInput(int usrInput);

int mainboxLimit = 1;

void matrixAnimationNcurses(WINDOW* win, const char* stringData, int startX, unsigned int characterDelay, unsigned int textDelay) {
    mainboxLimit++;
    if (mainboxLimit > 4)
    {
      mainboxLimit = 1;
      delwin(win);
      win = newwin(6, 70, 31, 31);
      box(textHud, 0, 0);
      mvwprintw(textHud, 0, 1, "Text");
      wrefresh(textHud);
    }

    srand(time(NULL));
    int len = strlen(stringData);
    char output[1024] = "";
    int out_len = 0;

    wrefresh(win);

    for (int i = 0; i < len; i++) {
        char realChar = stringData[i];

        // Flicker effect before final char
        for (int j = 0; j < 15; j++) {
            char randChar = (char)((rand() % 94) + 33);
            mvwprintw(win, mainboxLimit, startX + out_len, "%c", randChar);
            wrefresh(win);
            usleep(characterDelay);
        }

        // Print the actual character
        output[out_len++] = realChar;
        output[out_len] = '\0';

        mvwprintw(win, mainboxLimit, startX, "%s", output);
        wrefresh(win);
        usleep(textDelay);
    }

    wrefresh(win);
}

void battleStart()
{
    mvwprintw(mainScreen, 1, 0, "%s", battleBG);
    wrefresh(mainScreen);

    enemySprite = newwin(17, 35, 0, 0);
    box(enemySprite, 32, 32);
    center_box(mainScreen, enemySprite, 15);
    
    enemyHealthHud = newwin(7, 35, 0, 0);
    box(enemyHealthHud, 0, 0);
    center_box(mainScreen, enemyHealthHud, 8);

    char *battleChoices[] = {"Attack", "Defend", "Skill", "Item", "Run"};
    while (1)
    {
      int choices = usrInputChoices(battleChoices, commandHud, 1, 1);
      if (choices == 4) break;
    }
    wrefresh(mainScreen);
    wrefresh(enemyHealthHud);
    wrefresh(enemySprite);
}
