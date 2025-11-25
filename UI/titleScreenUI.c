#include <stdlib.h>
#include <string.h>

extern void center_box(WINDOW *parent, WINDOW *child, int y_offset, int styley, int stylex);
extern int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx, void (*onHighlight)(int index), bool isExtraKey);
extern void loadPlayer(struct Player *player);
WINDOW *titleScreenUI, *titleUI, *choicesUI, *nameInput;
extern bool isTutorial;

int titleInput(char* strChoices[], WINDOW *win)
{
    int starty = 1;
    int startx = 1;
    keypad(win, TRUE);
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
                if (highlight < 0) highlight = arraySize - 1;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= arraySize) highlight = 0;
                break;
            case 10: // Enter
                flushinp();
                return highlight;
            default:
                break;
        }

        flushinp();
        wrefresh(win);
    }
}

void inputPlayerName()
{
  clear();
  refresh();
  nameInput = newwin(5, 30, 0, 0);
  char *result = userInput(nameInput, "Siapa namamu?");
  strcpy(player.name, result);
}

void titleScreen()
{
  noecho();
  curs_set(0);
  titleScreenUI = newwin(39, 102, 0, 0);
  box(titleScreenUI, 0, 0);
  titleUI = newwin(6, 41, 0, 0);
  center_box(titleScreenUI, titleUI, 12, 32, 32);
  mvwprintw(titleUI, 0, 0, "░█████╗░████████╗██╗░░██╗███████╗██████╗░");
  mvwprintw(titleUI, 1, 0, "██╔══██╗╚══██╔══╝██║░░██║██╔════╝██╔══██╗");
  mvwprintw(titleUI, 2, 0, "███████║░░░██║░░░███████║█████╗░░██████╔╝");
  mvwprintw(titleUI, 3, 0, "██╔══██║░░░██║░░░██╔══██║██╔══╝░░██╔══██╗");
  mvwprintw(titleUI, 4, 0, "██║░░██║░░░██║░░░██║░░██║███████╗██║░░██║");
  mvwprintw(titleUI, 5, 0, "╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝╚══════╝╚═╝░░╚═╝");
  choicesUI = newwin(5, 20, 0, 0);
  center_box(titleScreenUI, choicesUI, 22, 0, 0);
  char *strChoices[] = {"New Game", "Load Game", "Exit", NULL};
  wrefresh(titleScreenUI);
  wrefresh(titleUI);
  int choices = titleInput(strChoices, choicesUI);
  switch (choices)
  {
      case 0:
        inputPlayerName();
        clear();
        break;
      case 1:
        loadPlayer(&player);
        isTutorial = false;
        clear();
        break;
      default:
        endwin();
        exit(0);
        break;
  }
}
