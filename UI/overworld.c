#include <stdlib.h>
#include <time.h>
#ifdef __unix__
    #include <ncurses.h>
#else
    #include <curses.h>
#endif
#include <unistd.h>
#include "battleUI.c"

extern void drawMainScreen();
extern int debugMessagePosition;
extern void inputDebugMessage(const char *messageString, ...);
extern void debugMenuInput(int usrInput);
extern bool isDebug;
extern int mainBoxLimit;
extern WINDOW * textHud;
extern void clearTextHud();
extern void matrixAnimationNcurses(int startX, unsigned int characterDelay, unsigned int textDelay, const char* stringData, ...);
extern void battleStart();
extern void clearMainScreen();

void drawTile(int y, int x, char tile)
{
    if (tile == 'W') {
        wattron(mainScreen, COLOR_PAIR(10));
        mvwaddch(mainScreen, y, x, tile);
        wattroff(mainScreen, COLOR_PAIR(10));
    }
    else if (tile == '.') {
        wattron(mainScreen, COLOR_PAIR(11));
        mvwaddch(mainScreen, y, x, tile);
        wattroff(mainScreen, COLOR_PAIR(11));
    }
    else if (tile == '~') {
        wattron(mainScreen, COLOR_PAIR(12));
        mvwaddch(mainScreen, y, x, tile);
        wattroff(mainScreen, COLOR_PAIR(12));
    }
    else {
        mvwaddch(mainScreen, y, x, tile);
    }
}

void onGrassEvent(char **map)
{
    inputDebugMessage("Player on grass");
    srand(time(NULL));
    int randomEncounter = rand() % 4;
    inputDebugMessage("randEn: %d", randomEncounter);

    if (randomEncounter == 1)
    {
        clearTextHud();
        matrixAnimationNcurses(1, 10, 10, "Sesuatu mendekat!");
        napms(150);
        battleStart();
        clearMainScreen();
    }

    // redraw map
    for (int i = 0; i < 29; i++) {
        for (int j = 0; j < 70 - 2; j++) {
            drawTile(i + 1, j + 1, map[i][j]);
        }
    }

    // Exit banner (red background + white text)
    for (int x = 1; x < 70 - 1; x++){
        wattron(mainScreen, COLOR_PAIR(4));
        mvwaddch(mainScreen, 1, x, '-');
        wattroff(mainScreen, COLOR_PAIR(4));
    }

    wattron(mainScreen, COLOR_PAIR(4));
    mvwprintw(mainScreen, 1, (70 / 2) - 5, "---EXIT---");
    wattroff(mainScreen, COLOR_PAIR(4));

    wrefresh(mainScreen);
    keypad(mainScreen, TRUE);
}

void generateMap(char **map, int h, int w)
{
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            map[y][x] = (rand() % 5 == 0) ? 'W' : '.'; // grass or dirt

    // random water pools
    for (int c = 0; c < 5; c++)
    {
        int gx = rand() % (w - 5);
        int gy = rand() % (h - 5);

        for (int y = gy; y < gy + 3; y++)
            for (int x = gx; x < gx + 3; x++)
                map[y][x] = '~';
    }
}

int overworldStart()
{
    // Grass (W)
    init_pair(10, COLOR_GREEN, COLOR_GREEN);

    // Dirt (.)
    init_pair(11, COLOR_BLACK, COLOR_YELLOW);

    // Water (~)
    init_pair(12, COLOR_WHITE, COLOR_BLUE);

    // Player (@)
    init_pair(20, COLOR_YELLOW, COLOR_WHITE);

    // EXIT banner -> white text on red background
    init_pair(4, COLOR_WHITE, COLOR_RED);

    // Default
    init_pair(5, COLOR_WHITE, COLOR_BLACK);

    int boxHeight, boxWidth;
    getmaxyx(mainScreen, boxHeight, boxWidth);
    keypad(mainScreen, TRUE);
    nodelay(mainScreen, FALSE);
    srand(time(NULL));

    // Allocate map
    char **map = malloc((boxHeight - 2) * sizeof(char *));
    for (int i = 0; i < boxHeight - 2; i++)
        map[i] = malloc((boxWidth - 2) * sizeof(char));

    generateMap(map, boxHeight - 2, boxWidth - 2);

    box(mainScreen, 0, 0);

    // Draw map
    for (int y = 0; y < boxHeight - 2; y++)
        for (int x = 0; x < boxWidth - 2; x++)
            drawTile(y + 1, x + 1, map[y][x]);

    // EXIT banner
    for (int x = 1; x < boxWidth - 1; x++){
        wattron(mainScreen, COLOR_PAIR(4));
        mvwaddch(mainScreen, 1, x, '-');
        wattroff(mainScreen, COLOR_PAIR(4));
    }

    wattron(mainScreen, COLOR_PAIR(4));
    mvwprintw(mainScreen, 1, (boxWidth / 2) - 5, "---EXIT---");
    wattroff(mainScreen, COLOR_PAIR(4));

    // Player start pos
    int px = boxWidth / 2;
    int py = boxHeight / 2;

    wattron(mainScreen, COLOR_PAIR(20));
    mvwaddch(mainScreen, py, px, '@');
    wattroff(mainScreen, COLOR_PAIR(20));
    wrefresh(mainScreen);

    int ch;
    while (1)
    {
        ch = wgetch(mainScreen);

        // restore tile under player
        drawTile(py, px, map[py - 1][px - 1]);

        debugMenuInput(ch);

        switch (ch)
        {
        case KEY_UP:    if (py > 1) py--; break;
        case KEY_DOWN:  if (py < boxHeight - 2) py++; break;
        case KEY_LEFT:  if (px > 1) px--; break;
        case KEY_RIGHT: if (px < boxWidth - 2) px++; break;
        }

        // exit
        if (py == 1)
        {
            mvwprintw(mainScreen, 0, 2, "[Exiting overworld...]");
            wrefresh(mainScreen);
            napms(400);
            break;
        }

        // grass event
        if (map[py - 1][px - 1] == 'W')
            onGrassEvent(map);

        // draw player
        wattron(mainScreen, COLOR_PAIR(20));
        mvwaddch(mainScreen, py, px, '@');
        wattroff(mainScreen, COLOR_PAIR(20));

        wrefresh(mainScreen);
        inputDebugMessage("x: %d y: %d", px, py);
    }

exit_overworld:
    for (int i = 0; i < boxHeight - 2; i++)
        free(map[i]);
    free(map);

    return 0;
}
