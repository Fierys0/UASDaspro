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
  #include <psapi.h>     // GetModuleFileNameExA
  #pragma comment(lib, "Psapi.lib") 
#endif

bool isSecret = false;

#ifdef _WIN32

BOOL is_legacy_console() {
    HWND hwnd = GetConsoleWindow();
    if (!hwnd) return FALSE;

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return FALSE;

    char path[MAX_PATH];
    DWORD size = MAX_PATH;

    if (!QueryFullProcessImageNameA(h, 0, path, &size)) {
        CloseHandle(h);
        return FALSE;
    }

    CloseHandle(h);

    return strstr(path, "conhost.exe") != NULL;
}

void setConsoleSize(int cols, int rows)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set buffer first
    COORD buffer;
    buffer.X = cols;
    buffer.Y = rows;

    if (!SetConsoleScreenBufferSize(hOut, buffer)) {
        return;
    }

    // Set window size second
    SMALL_RECT win = {0, 0, cols - 1, rows - 1};

    if (!SetConsoleWindowInfo(hOut, TRUE, &win)) {
        printf("SetConsoleWindowInfo error: %lu\n", GetLastError());
        return;
    }
}

void disableResize()
{
    HWND console = GetConsoleWindow();

    LONG style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_SIZEBOX;
    style &= ~WS_MAXIMIZEBOX;

    SetWindowLong(console, GWL_STYLE, style);

    SetWindowPos(console, NULL,
                 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    ShowScrollBar(console, SB_BOTH, FALSE);
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

  setConsoleSize(130, 37);

  #ifdef _WIN32
    disableResize();
    char* env_guard = getenv("GAME_ALREADY_LAUNCHED");

    // Intinya harus pake conhost.exe karena cmd.exe enggak bisa resize menggunakan api
    if (env_guard == NULL) {
        if (!is_legacy_console()) {
            char cmd[MAX_PATH];
            GetModuleFileNameA(NULL, cmd, MAX_PATH);

            char args[1024] = "";
            for (int i = 1; i < argc; i++) {
                // Buat args
                strcat(args, " ");      
                strcat(args, argv[i]);  
            }

            // Menjaga agar tidak mengclone diri sendiri
            _putenv("GAME_ALREADY_LAUNCHED=1");

            char launch[2048];
            sprintf(launch, "start \"Game Console\" conhost.exe cmd.exe /c \"%s\"%s", cmd, args);
            
            system(launch);
            return 0; 
        }
    }
    SetConsoleTitle("Ather");
  #endif

  setlocale(LC_ALL, "");
  initscr();
  initPlayer();
  initEnemies();
  titleScreen();
  mainUI(isSecret);
  return 0;
}