#include <stdlib.h>
#include <time.h>

int onGrass = 0;

void onGrassEvent(WINDOW *game)
{
    mvwprintw(game, 0, 2, "[You step on grass]");
    wrefresh(game);
    napms(150);
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
    for (int x = 1; x < boxWidth - 1; x++)
        mvwaddch(game, 1, x, '-');
    mvwprintw(game, 1, (boxWidth / 2) - 5, "---EXIT---");

    // Player position
    int px = boxWidth / 2;
    int py = boxHeight / 2;
    mvwaddch(game, py, px, '@');
    wrefresh(game);

    int ch;
    while (1)
    {
        ch = wgetch(game);

        // Restore old tile
        mvwaddch(game, py, px, map[py - 1][px - 1]);

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
        case 'q':
            goto exit_overworld;
        default:
            break;
        }

        // Trigger grass event
        if (map[py - 1][px - 1] == 'W')
            onGrassEvent(game);

        // Exit if touching the banner
        if (py == 1)
        {
            mvwprintw(game, 0, 2, "[Exiting overworld...]");
            wrefresh(game);
            napms(400);
            break;
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
