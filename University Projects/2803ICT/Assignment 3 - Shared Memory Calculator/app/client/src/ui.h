#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include <form.h>
#include <panel.h>
#include <stdlib.h>
#include <time.h>

typedef struct UI {
    WINDOW *windows[10];
    WINDOW *sub_windows[10];
    PANEL *panels[10];
    PANEL *sub_panels[10];
    WINDOW *progress_win;
    PANEL *progress_panel;
    WINDOW *input_win;
    FORM *form;
    FIELD *field[2];
} UI;

void end_ui(UI *ui);
void display_slots(WINDOW *sub_windows[10], uint32_t slot[10], char serverflag[10]);

void init_ui(UI *ui, char progress[10], int *current_window, struct timespec *start_times, struct timespec *finish_times);
void display_progress_bars(WINDOW *win, char progress[10], struct timespec *start_times, struct timespec *finish_times);
long long timespec_diff_ms(struct timespec *start, struct timespec *finish);

#endif