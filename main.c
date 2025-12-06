#include "game/core.c"
#include "UI/UI.c"
#include "UI/titleScreenUI.c"
#include <locale.h>
#ifdef __unix__
  #include <ncurses.h>
  #include <sys/ioctl.h>
#else
  #include <curses.h>
  #include <windows.h>
#endif

bool isSecret = false;

#ifdef _WIN32
void setConsoleSize(int cols, int rows)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // get screen buffer
    COORD bufferSize;
    bufferSize.X = cols;
    bufferSize.Y = rows;

    if (!SetConsoleScreenBufferSize(hOut, bufferSize)) {
        printf("Failed to set buffer size: %lu\n", GetLastError());
        return;
    }

    // set window size
    SMALL_RECT windowSize;
    windowSize.Left   = 0;
    windowSize.Top    = 0;
    windowSize.Right  = cols - 1;
    windowSize.Bottom = rows - 1;

    if (!SetConsoleWindowInfo(hOut, TRUE, &windowSize)) {
        printf("Failed to set window size: %lu\n", GetLastError());
        return;
    }
}

void disableResize()
{
    HWND console = GetConsoleWindow();
    LONG style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
}

#else
// i actually dont know if this works, i use hyprland and console size is fixed no matter what
void setConsoleSize(int cols, int rows)
{
    struct winsize ws;

    ws.ws_col = cols;  // width
    ws.ws_row = rows;  // height
    ws.ws_xpixel = 0;
    ws.ws_ypixel = 0;

    if (ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws) == -1) {
        perror("ioctl(TIOCSWINSZ)");
        return;
    }

    // Send SIGWINCH to this process
    kill(getpid(), SIGWINCH);
}
#endif

int main(int argc, char *argv[])
{
  if (argc > 1 && strcmp(argv[1], "debug") == 0) isSecret = true;

  setConsoleSize(120, 40);

  #ifdef _WIN32
    disableResize();
  #endif

  setlocale(LC_ALL, "");
  initscr();
  initPlayer();
  initEnemies();
  titleScreen();
  mainUI(isSecret);
  return 0;
}
