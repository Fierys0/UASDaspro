
WINDOW * enemySprite, * enemyHealthHud;

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
