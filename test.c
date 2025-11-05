#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 20
#define WIDTH 40

void onGrass(WINDOW *win) {
    mvwprintw(win, HEIGHT - 1, 2, "You’re walking on grass...   ");
    wrefresh(win);
}

// Generate a random map with square grass patches
void generateMap(char map[HEIGHT - 2][WIDTH - 2]) {
    // Fill map with '.'
    for (int y = 0; y < HEIGHT - 2; y++) {
        for (int x = 0; x < WIDTH - 2; x++) {
            map[y][x] = '.';
        }
    }

    int clusters = 6 + rand() % 5; // 6–10 grass patches

    for (int c = 0; c < clusters; c++) {
        int gx = rand() % (WIDTH - 6);  // left corner
        int gy = rand() % (HEIGHT - 6); // top corner

        int w = 2 + rand() % 4; // width 2–5
        int h = 2 + rand() % 4; // height 2–5

        // Fill rectangular patch
        for (int y = gy; y < gy + h && y < HEIGHT - 2; y++) {
            for (int x = gx; x < gx + w && x < WIDTH - 2; x++) {
                map[y][x] = 'W';
            }
        }
    }
}

int main() {
    srand(time(NULL));
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    int start_y = 1, start_x = 3;
    WINDOW *game = newwin(HEIGHT, WIDTH, start_y, start_x);
    keypad(game, TRUE);

    char map[HEIGHT - 2][WIDTH - 2];
    generateMap(map);

    // Draw initial map
    box(game, 0, 0);
    for (int y = 0; y < HEIGHT - 2; y++) {
        for (int x = 0; x < WIDTH - 2; x++) {
            mvwaddch(game, y + 1, x + 1, map[y][x]);
        }
    }

    // Player start position
    int px = WIDTH / 2, py = HEIGHT / 2;
    mvwaddch(game, py, px, '@');
    wrefresh(game);

    int ch;
    while ((ch = wgetch(game)) != 'q') {
        // Restore old tile
        mvwaddch(game, py, px, map[py - 1][px - 1]);

        // Move player
        switch (ch) {
            case KEY_UP:    if (py > 1) py--; break;
            case KEY_DOWN:  if (py < HEIGHT - 2) py++; break;
            case KEY_LEFT:  if (px > 1) px--; break;
            case KEY_RIGHT: if (px < WIDTH - 2) px++; break;
        }

        // Redraw player and border
        box(game, 0, 0);
        mvwaddch(game, py, px, '@');

        // Check if standing on grass
        if (map[py - 1][px - 1] == 'W')
            onGrass(game);
        else
            mvwprintw(game, HEIGHT - 1, 2, "                             ");

        wrefresh(game);
    }

    endwin();
    return 0;
}

