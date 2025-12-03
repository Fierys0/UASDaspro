#ifdef __unix__
  #include <ncurses.h>
#else
  #include <curses.h>
#endif
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include "sprite.c"
#include "overworld.c"
#include <stdarg.h>
#include "shopUI.c"

bool isDebug = true;
int debugPosition = 1;
int debugMessagePosition = 1;
WINDOW * lastKeypad;
bool isTutorial = true;

WINDOW *debugHud, *playerhud, *mainScreen, 
       *commandHud, *textHud, *debugMessageHud, 
       *inputUser, *debugCheat, *tutorialHud,
       *tutorialDesc;

int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx, void (*onHighlight)(int index), bool isExtraKey);
void debugMenuInput(int usrInput);
WINDOW *debugMenu();
void draw_all();
void handle_resize(int sig);
int exitMenu();
void drawTextHud();
void drawCommandHud();
void drawMainScreen();
void drawPlayerHud();
void drawDebugInput();
void drawDebugMessage();

void drawTutorial()
{
    char *tutorialString[] = {
      "Untuk bermain gunakan arrow key dan ketik enter untuk memilih",
      "Pilih explore untuk mengeksplore dan mencari musuh",
      "Pilih shop untuk berbelanja armor dan weapon untuk menambah damagemu!",
      "Rest untuk beristirahat dan memulihkan hp ke max hp kembali",
      "Save untuk menyimpan progressmu setelah explore atau membeli sesuatu",
      "Load untuk memuat perjalananmu sebelumnya",
      "Ketik escape [esc] untuk keluar dari program"
    };
    tutorialHud = newwin(10, 30, 0, 0);
    center_box(mainScreen, tutorialHud, 11, 0, 0);
    mvwprintw(tutorialHud, 2, 1, "Tutorial bermain");
    mvwprintw(tutorialHud, 0, 6, "Press Enter to continue");
    tutorialDesc = newwin(5, 28, 0, 0);
    center_box(mainScreen, tutorialDesc, 15, 32, 32);
    wrefresh(tutorialHud);
    wrefresh(tutorialDesc);
    for (int i = 0; i <= 6; i++)
    {
      mvwprintw(tutorialDesc, 0, 0, "%s", tutorialString[i]);
      wrefresh(tutorialDesc);
      while (1)
      {
        int pilihan = wgetch(tutorialHud);
        if (pilihan == 10) break;
      }
      werase(tutorialDesc);
    }
    werase(tutorialHud);
    werase(tutorialDesc);
}

void flashWindow(WINDOW *win, int flashes, int delay, int borderstyle)
{
    int h, w;
    getmaxyx(win, h, w);

    for (int i = 0; i < flashes; i++)
    {
        // Reverse border
        wattron(win, A_REVERSE);
        box(win, borderstyle, borderstyle);
        wattroff(win, A_REVERSE);
        wrefresh(win);
        napms(delay);

        // Normal border
        box(win, borderstyle, borderstyle);
        wrefresh(win);
        napms(delay);
    }
}

void debugMenuInput(int usrInput)
{
    if (!isDebug || !debugHud) return;

    mvwprintw(debugHud, debugPosition, 1, "                  ");
    switch (usrInput)
    {
        case 10:
            mvwprintw(debugHud, debugPosition, 1, "Enter");
            break;
        default:
            mvwprintw(debugHud, debugPosition, 1, "%c (%d)", (char)usrInput, usrInput);
            break;
    }

    debugPosition++;
    if (debugPosition >= 11)
    {
      debugPosition = 2;
      delwin(debugHud);
      drawDebugInput();
      mvwprintw(debugHud, 1, 1, "%c (%d)", (char)usrInput, usrInput);
    }
    box(debugHud, 0, 0);
    wrefresh(debugHud);
}

WINDOW *debugMenu()
{
  if (!isDebug) return NULL;
  drawDebugMessage();
  drawDebugInput();
}

void drawDebugInput()
{
  debugHud = newwin(11, 24, 26, 102);
  box(debugHud, 0, 0);
  mvwprintw(debugHud, 0, 1, "Debug Input");
  wrefresh(debugHud);
}

void drawDebugMessage()
{
  debugMessageHud = newwin(26, 24, 0 , 102);
  box(debugMessageHud, 0, 0);
  mvwprintw(debugMessageHud, 0, 1, "Debug Messages");
  wrefresh(debugMessageHud);
}

char* userInput(WINDOW *win, const char *question)
{
    int maxLen = 14;
    char *buffer = malloc(maxLen + 1);
    if (!buffer) return NULL;
    memset(buffer, 0, maxLen + 1);

    int pos = 0;

    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "%s", question);

    mvwprintw(win, 3, 2, "[              ]");
    wmove(win, 3, 3);
    wrefresh(win);

    curs_set(1);
    keypad(win, TRUE);

    int ch;
    while (1)
    {
        ch = wgetch(win);

        if (ch == '\n' || ch == KEY_ENTER) {
            break;
        }
        else if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && pos > 0) {
            pos--;
            buffer[pos] = '\0';
            mvwaddch(win, 3, 3 + pos, ' ');  // menghapus jika user backspace
            wmove(win, 3, 3 + pos);
        }
        else if (ch >= 32 && ch <= 126 && pos < maxLen) {
            buffer[pos] = ch;
            mvwaddch(win, 3, 3 + pos, ch);
            pos++;
        }

        wrefresh(win);
    }

    curs_set(0);
    return buffer;
}

// menunjukan cheat menu
void debugCheatMenu()
{
  debugCheat = newwin(5, 22, 0, 0);
  center_box(mainScreen, debugCheat, 13, 0, 0);
  mvwprintw(debugCheat, 0, 1, "Cheat(s)");
  wrefresh(debugCheat);
  char* strChoices[] = {"Max LVL and Gold", "Exit", NULL};
  while (1)
  {
    int choice = usrInputChoices(strChoices, debugCheat, 1, 1, NULL, false);

    if (choice == 0) 
    {
      player.money = 9999999;
      player.level = 99;
      break;
    } else if (choice == 1) break;
  }
  delwin(debugCheat);
  draw_all();
}

// fungsi untuk mengeprint di bagian window degug messages
void inputDebugMessage(const char *messageString, ...)
{
    if (!isDebug || !debugMessageHud) return;
    va_list args;
    va_start(args, messageString);

    wmove(debugMessageHud, debugMessagePosition, 1);
    vw_printw(debugMessageHud, messageString, args);

    va_end(args);

    debugMessagePosition++;
    if (debugMessagePosition >= 25)
    {
        debugMessagePosition = 2;
        delwin(debugMessageHud);
        drawDebugMessage();
        wmove(debugMessageHud, 1, 1);
        va_start(args, messageString);
        vw_printw(debugMessageHud, messageString, args);
        va_end(args);
    }

    wrefresh(debugMessageHud);
}

// center box function
void center_box(WINDOW *parent, WINDOW *child, int y_offset, int styley, int stylex)
{
    int parent_h, parent_w;
    int parent_y, parent_x;
    int child_h, child_w;
    int starty, startx;

    // untuk ngukur tengah harus ambil ukuran parent dan child
    getbegyx(parent, parent_y, parent_x);
    getmaxyx(parent, parent_h, parent_w);
    getmaxyx(child, child_h, child_w);

    // di center
    startx = parent_x + (parent_w - child_w) / 2;

    // offset sesuai parameter
    starty = y_offset;

    // Move and redraw the child
    mvwin(child, starty, startx);
    werase(child);
    box(child, styley, stylex);

    wrefresh(parent);
    wrefresh(child);
}

void clearCommandHud()
{
  delwin(commandHud);
  drawCommandHud();
}

void clearTextHud()
{
  delwin(textHud);
  drawTextHud();
}

void clearMainScreen()
{
  delwin(mainScreen);
  mainScreen = newwin(31, 70, 0, 31);
  box(mainScreen, 0, 0);
  wrefresh(mainScreen);
}

// fungsi untuk opsi pilihan dengan menggunakan arrow key
int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx, void (*onHighlight)(int index), bool isExtraKey)
{
    keypad(win, TRUE);
    int highlight = 0;
    int arraySize = 0;
    while (strChoices[arraySize] != NULL) arraySize++;

    if (onHighlight) onHighlight(highlight);

    if (shopForceHighlight >= 0) {
        highlight = shopForceHighlight;
        shopForceHighlight = -1;
    }

    while (1)
    {
        /* draw */
        for (int i = 0; i < arraySize; i++)
        {
            if (i == highlight) wattron(win, A_REVERSE);
            mvwprintw(win, i + starty, startx, "%s", strChoices[i]);
            if (i == highlight) wattroff(win, A_REVERSE);
        }
        wrefresh(win);

        if (isTutorial)
        {
            drawTutorial();
            isTutorial = false;
            drawMainScreen();
        }

        int oldHighlight = highlight;
        int usrInput = wgetch(win);

        switch (usrInput)
        {
            case KEY_UP:
                highlight = (highlight - 1 + arraySize) % arraySize;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % arraySize;
                break;
            case 10:
                /* Enter pressed */
                debugMenuInput( (usrInput==KEY_ENTER) ? KEY_ENTER : '\n' );
                return highlight;
            case 27: /* ESC */
                lastKeypad = commandHud;
                {
                    int exitChoices = exitMenu();
                    if (exitChoices == 1) {
                        endwin();
                        printf("Exited throught exithud");
                        exit(0);
                    }
                }
                break;
            case 47:
                if (isExtraKey)
                    return usrInput;
                break;

        }

        /* debug and highlight callback only when changed or key pressed */
        inputDebugMessage("hl: %d (%s)", highlight, strChoices[highlight]);
        debugMenuInput(usrInput);

        if (highlight != oldHighlight && onHighlight) onHighlight(highlight);
        flushinp();
  }
}

void drawPlayerHud()
{
    playerhud = newwin(15, 30, 0, 0);
    box(playerhud, 0, 0);
    mvwprintw(playerhud, 0, 1, "Status", player.name);
    mvwprintw(playerhud, 2, 1, "[%s]", player.name);
    mvwprintw(playerhud, 3, 1, "LV %d", player.level);
    mvwprintw(playerhud, 4, 1, "HP %s | %d/%d", playerHealthBar, player.health, player.maxHealth);
    mvwprintw(playerhud, 5, 1, "EXP %s | %d/%d", expBar, player.exp, levelUP);
    mvwprintw(playerhud, 6, 1, "Gold: %d", player.money);
    mvwprintw(playerhud, 7, 1, "Def: %d Agi: %d", player.defensePoint, player.agilityPoint);
    mvwprintw(playerhud, 8, 1, "Status Effect: None");
    mvwprintw(playerhud, 10, 1, "[Equipments]");
    mvwprintw(playerhud, 11, 1, "Armor: %s", player.armor.name);
    mvwprintw(playerhud, 12, 1, "Weapon: %s", player.weapon.name);
    wrefresh(playerhud);
}

void drawMainScreen()
{
    mainScreen = newwin(31, 70, 0, 31);
    box(mainScreen, 0, 0);
    int lines = sizeof(backgroundA) / sizeof(backgroundA[0]);
    for (int i = 1; i < lines; i++) 
      mvwprintw(mainScreen, i, 0, "%s", backgroundA[i]);
    wrefresh(mainScreen);
}

void drawCommandHud()
{
    commandHud = newwin(22, 30, 15, 0);
    box(commandHud, 0, 0);
    mvwprintw(commandHud, 0, 1, "Command");
    wrefresh(commandHud);
}

void drawTextHud()
{
    textHud = newwin(6, 70, 31, 31);
    box(textHud, 0, 0);
    mvwprintw(textHud, 0, 1, "Text");
    wrefresh(textHud);
}

// Beta dynamic UI vs Sigma static UI
void draw_all()
{
    clear();
    refresh();

    drawPlayerHud();
    drawMainScreen();
    drawCommandHud();
    drawTextHud();

    debugMenu();
}

// Menggambar wwindow untuk keluar program
int exitMenu()
{

    // Create popup window
    WINDOW *exitHud = newwin(7, 50, 15, 25);
    box(exitHud, 0, 0);
    mvwprintw(exitHud, 0, 22, "Exit");
    mvwprintw(exitHud, 2, 7, "Apakah kamu yakin ingin keluar game?");
    wrefresh(exitHud);

    keypad(exitHud, TRUE);

    char *strChoices[] = {"Tidak", "Ya"};
    int highlight = 0;

    while (1)
    {
        // Draw choices
        for (int i = 0; i < 2; i++)
        {
            int x = 19 + (i * 6);
            int y = 4;
            if (i == highlight)
              wattron(exitHud, A_REVERSE);
            mvwprintw(exitHud, y, x, "%s", strChoices[i]);
            wattroff(exitHud, A_REVERSE);
        }

        wrefresh(exitHud);

        int usrInput = wgetch(exitHud);

        switch (usrInput)
        {
        case KEY_LEFT:
            highlight--;
            if (highlight < 0)
                highlight = 0;
            break;

        case KEY_RIGHT:
            highlight++;
            if (highlight > 1)
                highlight = 1;
            break;

        case 10: // Enter key
        {
            int result = highlight;
            werase(exitHud);
            wrefresh(exitHud);
            delwin(exitHud);
            keypad(lastKeypad, TRUE);
            drawMainScreen(); drawCommandHud();
            return result; // 0 = No, 1 = Yes
        }

        case 27: // ESC key
        {
            werase(exitHud);
            wrefresh(exitHud);
            delwin(exitHud);
            keypad(lastKeypad, TRUE);
            drawMainScreen(); drawCommandHud();
            return 0;
        }

        default:
            break;
        }
    }
}

void handle_resize(int sig)
{
    endwin();
    printf("handle_resize");
    refresh();
    clear();
    #ifdef __unix__ 
      resizeterm(LINES, COLS);
    #endif
    draw_all();
    debugMenu();
}

int mainUI(bool isDebugArg)
{
    isDebug = isDebugArg;
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    refresh();
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_RED);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    #ifdef __unix__
      signal(SIGWINCH, handle_resize);
      set_escdelay(25);
    #endif
    draw_all();
    draw_all();

    sortArmorsByPrice();
    sortWeaponsByPrice();

    levelUP = (int)((float)(baseEXPUP * player.level * 1.50));

    char *menuChoices[] = {"Explore", "Shop", "Rest (5 Gold)", "Save", "Load", NULL};
    debugMenu();
    inputDebugMessage("Program Init");
    while (1)
    {
        playerHealthBar = drawBar(player.health, player.maxHealth);
        expBar = drawBar(player.exp, levelUP);
;
        drawPlayerHud();

        int choice = usrInputChoices(menuChoices, commandHud, 1, 1, NULL, true);
        if (choice == 47) 
          if(isDebug) debugCheatMenu();
        switch (choice)
        {
          case 0:
            clearCommandHud();
            clearMainScreen();
            overworldStart(mainScreen);
            drawMainScreen();
            break;
          case 1:
            clearCommandHud();
            startShop();
            break;
          case 2:
            clearCommandHud();
            playerRest(&player);
            break;
          case 3:
            savePlayer(&player);
            break;
          case 4:
            loadPlayer(&player);
            levelUP = (int)((float)(baseEXPUP * player.level * 1.50));
            break;
        }
        if (choice == -1) break;
    }

    endwin();
    printf("exit program?");
    return 0;
}
