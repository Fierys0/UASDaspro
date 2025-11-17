
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
extern void matrixAnimation(const char* stringData, unsigned int characterDelay, unsigned int textDelay);
extern void drawHealth(int health, int maxHealth);
extern void battleUI(struct Player player, struct entityData enemy);
extern struct Player startBattle(struct Player player, struct entityData enemy);
extern void battleAttack(struct Player* player, struct entityData* enemy);
extern struct entityData randomBattle();
extern void battleEnd(struct Player* player, struct entityData* enemy);
extern void mainMenu(struct Player player);
extern void gameOver();

int mainboxLimit = 1;

void matrixAnimationNcurses(WINDOW* win, int startX,
    unsigned int characterDelay, unsigned int textDelay,
    const char* stringData, ...)
{
    // Reset / recreate window if needed
    if (mainboxLimit > 4)
    {
        mainboxLimit = 1;
        delwin(win);
        win = newwin(6, 70, 31, 31);
        box(textHud, 0, 0);
        mvwprintw(textHud, 0, 1, "Text");
        wrefresh(textHud);
    }

    // Format the variadic string first
    char formatted[2048];

    va_list args;
    va_start(args, stringData);
    vsnprintf(formatted, sizeof(formatted), stringData, args);
    va_end(args);

    int len = strlen(formatted);
    char output[2048] = "";
    int out_len = 0;

    srand(time(NULL));
    wrefresh(win);

    for (int i = 0; i < len; i++) {
        char realChar = formatted[i];

        // Flicker animation
        for (int j = 0; j < 15; j++) {
            char randChar = (char)((rand() % 94) + 33);
            mvwprintw(win, mainboxLimit, startX + out_len, "%c", randChar);
            wrefresh(win);
            usleep(characterDelay);
        }

        // Reveal real char
        output[out_len++] = realChar;
        output[out_len] = '\0';

        mvwprintw(win, mainboxLimit, startX, "%s", output);
        wrefresh(win);
        usleep(textDelay);
    }

    mainboxLimit++;
    wrefresh(win);
}

void drawHealthUI(struct entityData enemy)
{
    enemyHealthHud = newwin(7, 31, 0, 0);
    center_box(mainScreen, enemyHealthHud, 6, 0, 0);
    mvwprintw(enemyHealthHud, 1, 1, "%s (LV %d)", enemy.name, enemy.level);
    mvwprintw(enemyHealthHud, 2, 1, "%d/%d", enemy.health, enemy.maxHealth);
    wrefresh(enemyHealthHud);
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

    drawHealthUI();

    clearCommandHud();
    char *battleChoices[] = {"Attack", "Defend", "Skill", "Item", "Run", NULL};
    while (1)
    {
      int choices = usrInputChoices(battleChoices, commandHud, 1, 1);
      if (choices == 4) break;
    }

    clearCommandHud();
}
