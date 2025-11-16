// title_pro.c
#define _XOPEN_SOURCE_EXTENDED 1
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <stdio.h>

const wchar_t *titleW =
L"░█████╗░████████╗██╗░░██╗███████╗██████╗░\n"
L"██╔══██╗╚══██╔══╝██║░░██║██╔════╝██╔══██╗\n"
L"███████║░░░██║░░░███████║█████╗░░██████╔╝\n"
L"██╔══██║░░░██║░░░██╔══██║██╔══╝░░██╔══██╗\n"
L"██║░░██║░░░██║░░░██║░░██║███████╗██║░░██║\n"
L"╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝╚══════╝╚═╝░░╚═╝";

int main(void) {
    // Ensure UTF-8 capable locale
    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "Warning: locale not set, UTF-8 may not work.\n");
    }

    // Start curses
    initscr();
    if (has_colors()) start_color();
    cbreak();
    noecho();
    curs_set(0);

    // Fixed window size
    const int WIN_H = 39;
    const int WIN_W = 102;
    const int START_Y = 0;
    const int START_X = 0;

    // Check actual terminal size — if too small, fail early with clear message
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);
    if (term_rows < WIN_H || term_cols < WIN_W) {
        endwin();
        fprintf(stderr,
            "Terminal too small for fixed window %dx%d. Current terminal: %dx%d\n"
            "Resize terminal (at least %d rows and %d cols) and try again.\n",
            WIN_H, WIN_W, term_rows, term_cols, WIN_H, WIN_W);
        return 2;
    }

    // Create the fixed-size window at top-left
    WINDOW *win = newwin(WIN_H, WIN_W, START_Y, START_X);
    if (!win) {
        endwin();
        fprintf(stderr, "Failed to create window (newwin returned NULL)\n");
        return 3;
    }

    // enable arrow keys on this window
    keypad(win, TRUE);

    // Draw border
    box(win, 0, 0);

    // Prepare wide-string lines
    wchar_t *copy = wcsdup(titleW);
    if (!copy) {
        delwin(win);
        endwin();
        fprintf(stderr, "Memory allocation failed (wcsdup)\n");
        return 4;
    }

    // Tokenize into lines using wcstok
    wchar_t *lines[32];
    int nlines = 0;
    wchar_t *saveptr = NULL;
    wchar_t *tok = wcstok(copy, L"\n", &saveptr);
    size_t max_line_len = 0;
    while (tok && nlines < (int)(sizeof(lines)/sizeof(lines[0]))) {
        lines[nlines++] = tok;
        size_t l = wcslen(tok);
        if (l > max_line_len) max_line_len = l;
        tok = wcstok(NULL, L"\n", &saveptr);
    }

    // If no lines extracted, bail
    if (nlines == 0) {
        free(copy);
        delwin(win);
        endwin();
        fprintf(stderr, "No title lines extracted. Aborting.\n");
        return 5;
    }

    // Center title block inside the fixed window
    int title_top = (WIN_H - nlines) / 3;
    if (title_top < 1) title_top = 1;

    // Use mvwaddwstr to print wide strings (truncates if beyond window)
    for (int i = 0; i < nlines; ++i) {
        int x = (int)((WIN_W - (int)wcslen(lines[i])) / 2);
        if (x < 1) x = 1;
        // Ensure we don't write outside the window width: use a temporary buffer truncated to WIN_W-2 chars
        // But mvwaddwstr will internally clip; still we compute safe col.
        mvwaddwstr(win, title_top + i, x, lines[i]);
    }

    // Menu setup
    const char *choices[] = {"New Game", "Load  Game", "Exit"};
    const int nchoices = sizeof(choices) / sizeof(choices[0]);
    int highlight = 0;
    int ch;

    // Initial draw of menu and instructions
    while (1) {
        // Redraw box + title area (title already printed, but we clear and redraw to avoid artifacts)
        werase(win);
        box(win, 0, 0);

        // Title
        for (int i = 0; i < nlines; ++i) {
            int x = (int)((WIN_W - (int)wcslen(lines[i])) / 2);
            if (x < 1) x = 1;
            mvwaddwstr(win, title_top + i, x, lines[i]);
        }

        // Menu: centered below title
        int menu_y = title_top + nlines + 3;
        for (int i = 0; i < nchoices; ++i) {
            int len = (int)strlen(choices[i]);
            int x = (WIN_W - len) / 2;
            if (i == highlight) {
                wattron(win, A_REVERSE);
                mvwprintw(win, menu_y + i*2, x, "%s", choices[i]);
                wattroff(win, A_REVERSE);
            } else {
                mvwprintw(win, menu_y + i*2, x, "%s", choices[i]);
            }
        }

        // Instructions
        const char *instr = "Use ↑/↓ to move and Enter to select (ESC = Exit)";
        mvwprintw(win, WIN_H - 2, (WIN_W - (int)strlen(instr)) / 2, "%s", instr);

        wrefresh(win);

        ch = wgetch(win);
        if (ch == KEY_UP) {
            highlight = (highlight - 1 + nchoices) % nchoices;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % nchoices;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            break;
        } else if (ch == 27) {
            // ESC pressed -> treat as Exit
            highlight = 1;
            break;
        }
    }

    // Clean up
    free(copy);
    delwin(win);
    endwin();

    // Final selection printed to stdout
    printf("Selected: %s\n", choices[highlight]);
    return 0;
}

