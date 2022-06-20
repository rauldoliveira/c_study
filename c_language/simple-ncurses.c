#include <stdio.h>
#include <ncurses.h>
#include <limits.h>         /* INT_MAX */
#include <unistd.h>         /* sleep */

int main (void)
{
    int i = 0;
    WINDOW * w1, * w2;
    initscr();

    w1 = newwin(LINES / 2, COLS, 0, 0);
    w2 = newwin(LINES / 2, COLS, (LINES / 2) + 1, 0);

    while (1) {
        wclear(w1);
        wclear(w2);
        if (++i == INT_MAX - 1) {
            wprintw(w1, "counter overflow, restart it.");
            i = 0;
            continue;
        }
        wprintw(w1, "counter: %i (lines = %i, cols = %i)", i, LINES, COLS);
        wprintw(w2, "shifted: %i", i + 666);
        wrefresh(w1);
        wrefresh(w2);
        sleep(1);
    }

    return 0;
}