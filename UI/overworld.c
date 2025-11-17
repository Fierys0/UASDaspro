#include <stdlib.h>
#include <time.h>
#include "battleUI.c"

extern void drawMainScreen();
extern int debugMessagePosition;
extern void inputDebugMessage(const char *messageString, ...);
extern void debugMenuInput(int usrInput);
extern bool isDebug;
extern int mainBoxLimit;
extern void matrixAnimationNcurses(WINDOW* win, const char* stringData, int startX, unsigned int characterDelay, unsigned int textDelay);
int onGrass = 0;
extern WINDOW * textHud;
extern void clearTextHud();

void battleStart();

void onGrassEvent(WINDOW *game, char **map)
{
    inputDebugMessage("Player on grass");
    keypad(game, FALSE);
    srand(time(NULL));
    int randomEncounter = rand() % 4;
    inputDebugMessage("randEn: %d", randomEncounter);
    if (randomEncounter == 1)
    {
      struct entityData enemy;
      enemy = randomBattle;
      clearTextHud();
      matrixAnimationNcurses(textHud, 1, 1500, 1500, "Sesuatu mendekat!");
      usleep(1500000);
      battleStart(player, enemy);
    }
    drawMainScreen();
    for (int i = 0; i < 29; i++) {
      mvwprintw(game, i + 1, 1, "%s", map[i]);
    }
    for (int x = 1; x < 70 - 1; x++){
      attron(COLOR_PAIR(4));
      mvwaddch(game, 1, x, '-');
      attron(COLOR_PAIR(4));
      wrefresh(game);
    }
    attron(COLOR_PAIR(4));
    mvwprintw(game, 1, (70 / 2) - 5, "---EXIT---");
    attron(COLOR_PAIR(4));

    wrefresh(game);
    keypad(game, TRUE);
}

void generateMap(char **map, int h, int w)
{
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            map[y][x] = (rand() % 5 == 0) ? 'W' : '.'; // grass (W) or dirt

    // create some random water patches for variety
    for (int c = 0; c < 5; c++)
    {
        int gx = rand() % (w - 5);
        int gy = rand() % (h - 5);
        for (int y = gy; y < gy + 3; y++)
            for (int x = gx; x < gx + 3; x++)
                map[y][x] = '~';
    }
}

int overworldStart(WINDOW *game)
{
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_RED);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    int boxHeight, boxWidth;
    getmaxyx(game, boxHeight, boxWidth);
    keypad(game, TRUE);
    nodelay(game, FALSE);
    srand(time(NULL));

    // Allocate map memory
    char **map = malloc((boxHeight - 2) * sizeof(char *));
    for (int i = 0; i < boxHeight - 2; i++)
        map[i] = malloc((boxWidth - 2) * sizeof(char));

    generateMap(map, boxHeight - 2, boxWidth - 2);

    // Draw border box
    box(game, 0, 0);

    // Draw map content
    for (int y = 0; y < boxHeight - 2; y++)
        for (int x = 0; x < boxWidth - 2; x++)
            mvwaddch(game, y + 1, x + 1, map[y][x]);

    // Draw the exit banner (inside the box)
    for (int x = 1; x < boxWidth - 1; x++){
      attron(COLOR_PAIR(4));
      mvwaddch(game, 1, x, '-');
      attron(COLOR_PAIR(4));
    }
    attron(COLOR_PAIR(4));
    mvwprintw(game, 1, (boxWidth / 2) - 5, "---EXIT---");
    attron(COLOR_PAIR(4));

    // Player position
    int px = boxWidth / 2;
    int py = boxHeight / 2;
    attron(COLOR_PAIR(2));
    mvwaddch(game, py, px, '@');
    attron(COLOR_PAIR(2));
    wrefresh(game);

    int ch;
    while (1)
    {
        ch = wgetch(game);

        // Restore old tile
        mvwaddch(game, py, px, map[py - 1][px - 1]);
        debugMenuInput(ch);

        switch (ch)
        {
        case KEY_UP:
            if (py > 1)
                py--;
            break;
        case KEY_DOWN:
            if (py < boxHeight - 2)
                py++;
            break;
        case KEY_LEFT:
            if (px > 1)
                px--;
            break;
        case KEY_RIGHT:
            if (px < boxWidth - 2)
                px++;
            break;
        default:
            break;
        }
        // Exit if touching the banner
        if (py == 1)
        {
            mvwprintw(game, 0, 2, "[Exiting overworld...]");
            wrefresh(game);
            napms(400);
            break;
        }

        // Trigger grass event
        if (map[py - 1][px - 1] == 'W')
        {
          keypad(game, FALSE);
          onGrassEvent(game, map);
        }

        // Draw player
        mvwaddch(game, py, px, '@');
        wrefresh(game);
    }

exit_overworld:
    for (int i = 0; i < boxHeight - 2; i++)
        free(map[i]);
    free(map);
    return 0;
}
