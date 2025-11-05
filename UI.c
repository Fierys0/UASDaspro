#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

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

const char* backgroundA =
"│     │  │ ││  |  `._|     | `'--.--'´ |     |_.´  |   :|  | .´ |  | │"
"│--.- │  │ ││:,|   | `'-,._|     |     |_.,-'´ |   |,:´ |  :´|  |  | │"
"│  |  │  │,││|  `. |    |   `':--'--:'´   |    | .´  |  |.|  |  |  | │"
"│  |  │  │∞││|   |`'-.,_|     |     |     |_,.-'´|   |.:´ |  |  |  :.│"
"│--'- │  │(││',  |   |   `':--'--.--'--:'´   |   |  ,' |  |  |  !.´ |│"
"│     │  │)││  |´'-.,|     |     |     |     |,.-'`|   |  | ,'¡´ |  |│"
"│     │  │ ││  |   |   `¡·-'--.--'--.--'-·¡'´  |   |   |_.'|  |  |  |│"
"│--.- │  !│││¡·!_  |    |     |     |     |    |  _!·¡'´|  |  |  |  |│"
"│  |  │    '│|   |¯ `'¡''·-.--'--.--'--.-·''¡'´ ¯|   |  |  |  |  |  |│"
"│  |  │     │|   |    |    |     |     |    |    |   |  |  |  |  |  |│"
"│--'- │     │'-.-'-.--'-.--'--┬--'--┬--'--.-'--.-'-.-'-.'-.'-.'-.'-.'│"
"│     │     │  |   |    |     |     |     |    |   |   |  |  |  |  | │"
"│     │     │  |  _|,...·--.--'--.--'--.--·...,|_  |   |  |  |  |  | │"
"│--.- │     │:'´¯|    |    |     '     |    |    |¯`':-:._!  |  |  | │"
"│  |  │     │|   |  _.!,..-'--.--'--.--'-..,!._  |   |  |  |`'-.|  | │"
"│  |  │     │!.:'´¯|    |   __! _ _ !__   |    |¯`':.!_ |  |  | `:.| │"
"│--'┌─┴─────┴─┐|  _!.--' ´¯   \\       .´ ¯`'--.!_  |   |`'.|  |  |  ¡│"
"│   └╥┬──┬──┬┬┘'´    `'-._     \\    .´        __.-`'-._|  |`. |  |  |│"
"│·┌──╨┴──┴──┴┴──┐___      `'-.\".·- -·.\"_  .-'´         `-.|  |!  |  |│"
"│ └─────────────┘    ¯¯`' .:¡|'| ' | '| ¡:.               `, | `.|  |│"
"│¯¯¯¯¯¯\\¯¯¯¯¯¯\\¯¯¯¯¯¯\\¯¯.||||  |     | ||'||.¯¯ ¯¯¯ ¯¯¯ ¯¯ ¯`!  |`. |│"
"│¯¯¯\\¯¯¯¯¯¯\\¯¯¯¯¯¯\\¯¯¯¯¯||| | ||  |     |  ||                 · |  ¡:│"
"│\\¯¯¯¯¯¯¯¯¯¯¯¯¯\\¯¯¯¯¯¯¯¯\\|  |  '          ||/'¯¯ ¯¯¯ ¯¯ ¯¯¯ ¯¯ `·  | │"
"│_\\_____________\\________`,             | ,'                     ` | │"
"│_____\\______\\______\\____/  `-, _    _.-´¯\\_.-|\\¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\\ │"
"│__\\______\\______\\______\\______\\ ¯¯¯¯ \\_____\\_\\ \\|\\¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯│"
"│      \\      \\      \\      \\   ¯¯¯\\¯¯    \\    \\   \\|\\¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯│"
"│`-────-`-────-`-────-`-────-`-────-`-────-`-───\\     \\|\\¯¯¯¯¯¯¯¯¯¯¯¯│"
;

WINDOW * debugHud, * playerhud, * mainScreen, * commandHud, * textHud;

WINDOW * debugMenu()
{
  if (isDebug){
    debugHud = newwin(37, 20, 0, 102);
    box(debugHud, 0, 0);
    mvwprintw(debugHud, 0, 1, "Debug");
    wrefresh(debugHud);
    return debugHud;
  }
}

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

    debugMenu();
}

void handle_resize(int sig)
{
    endwin();
    refresh();
    clear();
    resizeterm(LINES, COLS);
    draw_all();
}

int main()
{
    setlocale(LC_ALL, "");
    initscr();
    ESCDELAY = 25;
    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, TRUE);
    refresh();

    signal(SIGWINCH, handle_resize);
    draw_all();

    char *menuChoices[] = {"Explore","Shop","Exit"};
    debugHud = debugMenu();


    commandHud = newwin(22, 30, 15, 0);
    box(commandHud, 0, 0);
    mvwprintw(commandHud, 0, 1, "Command");
    wrefresh(commandHud);

    keypad(commandHud, true);
    int debugPosition = 1;
    int highlight = 0;
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
          if (i == highlight) {wattron(commandHud, A_REVERSE);}
          mvwprintw(commandHud, i+2, 1, menuChoices[i]);
          wattroff(commandHud, A_REVERSE);
        }
        int usrInput = wgetch(commandHud);
        if (debugPosition >= 36)
            debugPosition = 1;

        switch (usrInput)
        {
          case KEY_UP:
            highlight--;
            if(highlight == -1)
            {
              highlight = 0;
            }
            break;
          case KEY_DOWN:
            highlight++;
            if(highlight == 3)
            {
              highlight = 2;
            }
            break;
          case 27:
            return 1;
          default:
            break;
        }

        mvwprintw(debugHud, debugPosition, 1, "                  ");
        switch (usrInput)
        {
            case 10:
                mvwprintw(debugHud, debugPosition, 1, "Enter");
                break;
            default:
                mvwprintw(debugHud, debugPosition, 1, "Input: %c (%d)", (char)usrInput, usrInput);
                break;
        }
        debugPosition++;
        box(debugHud, 0, 0);
        mvwprintw(debugHud, 0, 1, "Debug");
        wrefresh(debugHud);
        wrefresh(commandHud);
    }

    endwin();
    return 0;
}

