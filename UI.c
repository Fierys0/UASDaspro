#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "sprite.c"
#include "test.c"

#ifdef _WIN32
#include <windows.h>
void resize_console(int width, int height)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rect = {0, 0, width - 1, height - 1};
    COORD coord = {width, height};
    SetConsoleScreenBufferSize(h, coord);
    SetConsoleWindowInfo(h, TRUE, &rect);
}
#else
void resize_console(int cols, int rows)
{
    printf("\033[8;%d;%dt", rows, cols);
    fflush(stdout);
}
#endif

bool isDebug = true;
int debugPosition = 1;
WINDOW * lastKeypad;

WINDOW *debugHud, *playerhud, *mainScreen, *commandHud, *textHud, *debugMessageHud;

int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx);
void debugMenuInput(int usrInput);
WINDOW *debugMenu();
void draw_all();
void handle_resize(int sig);
int exitMenu();
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
      debugMenu();
      mvwprintw(debugHud, 1, 1, "%c (%d)", (char)usrInput, usrInput);
    }
    wrefresh(debugHud);
}

WINDOW *debugMenu()
{
  if (!isDebug) return NULL;
  debugMessageHud = newwin(26, 20, 0 , 102);
  box(debugMessageHud, 0, 0);
  mvwprintw(debugMessageHud, 0, 1, "Debug Messages");
  wrefresh(debugMessageHud);
  debugHud = newwin(11, 20, 26, 102);
  box(debugHud, 0, 0);
  mvwprintw(debugHud, 0, 1, "Debug Input");
  wrefresh(debugHud);
}

int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx)
{
    int highlight = 0;
    int arraySize = 0;
    while (strChoices[arraySize] != NULL)
        arraySize++;

    while (1)
    {
        flushinp();
        for (int i = 0; i < arraySize; i++)
        {
            if (i == highlight)
                wattron(win, A_REVERSE);
            mvwprintw(win, i + starty, startx, "%s", strChoices[i]);
            wattroff(win, A_REVERSE);
        }

        int usrInput = wgetch(win);

        switch (usrInput)
        {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= arraySize) highlight = arraySize - 1;
                break;
            case 10: // Enter
                flushinp();
                return highlight;
            case 27: // ESC
                lastKeypad = commandHud;
                int exitChoices = exitMenu();
                flushinp();
                if (exitChoices == 1) return -1;
                break;
            default:
                break;
        }

        flushinp();
        debugMenuInput(usrInput);
        wrefresh(win);
    }
}

// Beta dynamic UI vs Sigma static UI
void draw_all()
{
    clear();
    refresh();

    playerhud = newwin(15, 30, 0, 0);
    box(playerhud, 0, 0);
    mvwprintw(playerhud, 0, 1, "Player");
    mvwprintw(playerhud, 2, 1, "[Status]");
    mvwprintw(playerhud, 3, 1, "LV 10");
    mvwprintw(playerhud, 4, 1, "HP [##########] | 100/100");
    mvwprintw(playerhud, 5, 1, "EXP [##########] | 10/300");
    mvwprintw(playerhud, 6, 1, "Money: 3000");
    mvwprintw(playerhud, 7, 1, "Def: 10 Agi: 10");
    mvwprintw(playerhud, 8, 1, "Status Effect: None");
    mvwprintw(playerhud, 10, 1, "[Equipments]");
    mvwprintw(playerhud, 11, 1, "Armor: Cloth");
    mvwprintw(playerhud, 12, 1, "Weapon: Iron Sword");
    wrefresh(playerhud);

    mainScreen = newwin(31, 70, 0, 31);
    box(mainScreen, 0, 0);
    mvwprintw(mainScreen, 1, 0, "%s", backgroundA);
    wrefresh(mainScreen);

    commandHud = newwin(22, 30, 15, 0);
    box(commandHud, 0, 0);
    mvwprintw(commandHud, 0, 1, "Command");
    wrefresh(commandHud);

    textHud = newwin(6, 70, 31, 31);
    box(textHud, 0, 0);
    mvwprintw(textHud, 0, 1, "Text");
    wrefresh(textHud);
}

int exitMenu()
{
    // Disable keypad for main window to avoid input conflict
    keypad(lastKeypad, FALSE);

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
            keypad(exitHud, FALSE);
            int result = highlight;
            werase(exitHud);
            wrefresh(exitHud);
            delwin(exitHud);
            keypad(lastKeypad, TRUE);
            draw_all();
            debugMenu();
            return result; // 0 = No, 1 = Yes
        }

        case 27: // ESC key
        {
            keypad(exitHud, FALSE);
            werase(exitHud);
            wrefresh(exitHud);
            delwin(exitHud);
            keypad(lastKeypad, TRUE);
            draw_all();
            debugMenu();
            return 0; // Treat ESC as "No"
        }

        default:
            break;
        }
    }
}

void handle_resize(int sig)
{
    endwin();
    refresh();
    clear();
    resizeterm(LINES, COLS);
    draw_all();
}

int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    set_escdelay(25);
    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, TRUE);
    refresh();
    raw();

    signal(SIGWINCH, handle_resize);
    draw_all();

    char *menuChoices[] = {"Explore", "Shop", "Save", "Load", NULL};
    keypad(commandHud, TRUE);

    debugMenu();
    while (1)
    {
        int choice = usrInputChoices(menuChoices, commandHud, 1, 1);
        if (choice == 0) overworldStart(mainScreen);
        if (choice == -1) break; // ESC pressed
    }

    endwin();
    return 0;
}
