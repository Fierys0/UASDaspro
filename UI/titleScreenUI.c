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
                return highlight;
            default:
                break;
        }

        wrefresh(win);
    }
}

void inputPlayerName()
{
  clear();
  refresh();
  nameInput = newwin(5, 30, 0, 0);
  char *result;
  result = userInput(nameInput, "Siapa namamu?");
  if (result[0] == '\0')
    strcpy(player.name, "Hero");
  else 
    strcpy(player.name, result);
}

void titleScreen()
{
  int min_rows = 39;    // Minimum required rows
  int min_cols = 111;    // Minimum required columns

  int rows, cols;
  getmaxyx(stdscr, rows, cols); // Get current terminal size

  if (rows < min_rows || cols < min_cols) {
      endwin(); // End ncurses mode before printing normally
      printf("Terminal is too small! Minimum required: %dx%d. Current: %dx%d\n",
             min_cols, min_rows, cols, rows);
      #ifdef _WIN32
        system("pause");
      #endif
      exit(0);
  }
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
