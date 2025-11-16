
WINDOW * enemySprite, * enemyHealthHud;

extern WINDOW * mainScreen, * textHud, *commandHud;
extern bool isDebug;
extern void center_box(WINDOW *parent, WINDOW *child, int y_offset, int styley, int stylex);
extern int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx);
extern void debugMenuInput(int usrInput);
extern WINDOW *debugMenu();
extern void draw_all();
extern void handle_resize(int sig);
extern int exitMenu();
extern void debugMenuInput(int usrInput);
extern void drawTextHud();
extern void drawCommandHud();
extern void drawMainScreen();
extern void drawPlayerHud();
extern int debugMessagePosition;
extern void inputDebugMessage(const char *messageString, ...);
extern const char* slimeSprite;
extern void clearCommandHud();

int mainboxLimit = 1;

void matrixAnimationNcurses(WINDOW* win, const char* stringData, int startX, unsigned int characterDelay, unsigned int textDelay) {
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
    mainboxLimit++;

    wrefresh(win);
}

void battleStart()
{
    mvwprintw(mainScreen, 1, 0, "%s", battleBG);
    wrefresh(mainScreen);
    // Draw the sprite safely
    
    enemySprite = newwin(14, 30, 0, 0);
    center_box(mainScreen, enemySprite, 12, 0, 0);
    mvwaddstr(enemySprite, 1, 0, goblinSprite); // <-- use mvwaddstr, not mvwprintw
    wrefresh(enemySprite);

    enemyHealthHud = newwin(7, 31, 0, 0);
    center_box(mainScreen, enemyHealthHud, 6, 0, 0);
    mvwprintw(enemyHealthHud, 1, 1, "Slime");
    wrefresh(enemyHealthHud);
    
    clearCommandHud();
    char *battleChoices[] = {"Attack", "Defend", "Skill", "Item", "Run", NULL};
    while (1)
    {
      int choices = usrInputChoices(battleChoices, commandHud, 1, 1);
      if (choices == 4) break;
    }

    clearCommandHud();
}
