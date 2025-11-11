#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void matrixAnimationNcurses(WINDOW* win, const char* stringData, unsigned int characterDelay, unsigned int textDelay) {
    srand(time(NULL));
    int len = strlen(stringData);
    char output[1024] = "";
    int out_len = 0;

    // Get window dimensions
    int maxY, maxX;
    getmaxyx(win, maxY, maxX);

    // Compute start position (centered horizontally)
    int startY = maxY / 2;
    int startX = (maxX - len) / 2;

    box(win, 0, 0);
    wrefresh(win);

    for (int i = 0; i < len; i++) {
        char realChar = stringData[i];

        // Show random flickering characters before the real one appears
        for (int j = 0; j < 15; j++) {
            char randChar = (char)((rand() % 94) + 33);
            mvwprintw(win, startY, startX + out_len, "%c", randChar);
            wrefresh(win);
            usleep(characterDelay);
        }

        // Print the actual character
        output[out_len++] = realChar;
        output[out_len] = '\0';

        mvwprintw(win, startY, startX, "%s", output);
        wrefresh(win);
        usleep(textDelay);
    }

    // Wait before exit (optional)
    mvwprintw(win, startY + 2, startX, "Press any key to continue...");
    wrefresh(win);
    wgetch(win);
}

int main() {
    initscr();
    noecho();
    curs_set(0);

    // Create a box window
    int height = 10, width = 80;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;
    WINDOW* boxWin = newwin(height, width, startY, startX);
    box(boxWin, 0, 0);
    wrefresh(boxWin);

    const char* text = "Matrix effect inside ncurses!";
    matrixAnimationNcurses(boxWin, text, 20000, 70000);

    delwin(boxWin);
    endwin();
    return 0;
}

