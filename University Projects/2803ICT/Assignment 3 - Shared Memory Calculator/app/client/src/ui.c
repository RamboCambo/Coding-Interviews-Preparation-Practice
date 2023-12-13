#include "ui.h"


void init_ncurses()
{
    initscr();
    noecho();
    cbreak();
    start_color();
    keypad(stdscr, TRUE);
    curs_set(0);
}

void init_colours() {
    // Initialize colors
    for (int i = 1; i <= 10; i++)
    {
        init_pair(i, COLOR_BLACK, i); // setting foreground as black and varying the background color.
    }
}

void display_slots(WINDOW *win[10], uint32_t slot[10], char serverflag[10]) {
    for (int i = 0; i < 10; i++) {
        if (serverflag[i] == 1) {
            // wclear(win[i]);
            wprintw(win[i], "%u\n", slot[i]);
        }
    }
    update_panels();
    doupdate();
}

long long timespec_diff_ms(struct timespec *start, struct timespec *finish)
{
    long long diff_sec = finish->tv_sec - start->tv_sec;
    long long diff_nsec = finish->tv_nsec - start->tv_nsec;
    long long diff_ms = diff_sec * 1000 + diff_nsec / 1000000;
    return diff_ms;
}

void display_progress_bars(WINDOW *win, char progress[10], struct timespec *start_times, struct timespec *finish_times)
{
    // Erase previous content, necessary for clean refresh
    werase(win);

    for (int i = 0; i < 10; i++)
    {
        int width = 20 * progress[i] / 32;
        int percentage = 100 * width / 20;

        mvwprintw(win, i + 1, 2, "Progress %d: %3d%%", i, percentage);

        for (int j = 0; j < width; j++)
        {
            // Use a full block character to fill the progress bar
            mvwaddch(win, i + 1, j + 19, '#');
        }

        if(progress[i] < 32)
        {
            clock_gettime(CLOCK_REALTIME, &finish_times[i]);
        }

        long long diff_ms = timespec_diff_ms(&start_times[i], &finish_times[i]);
        long long rounded_num = (diff_ms / 500) * 500;
        mvwprintw(win, i + 1, 40, "%10lldms", rounded_num);
    }

    // Redraw the box after updating the progress bars
    box(win, 0, 0);

    wrefresh(win);
}

void create_windows(UI *ui, int COL, int ROW) {
    int x = 0;

    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            x = COL / 3;
        } else {
            x = 2 * COL / 3;
        }

        ui->windows[i] = newwin(ROW, COL / 3, 0, x);
        wbkgd(ui->windows[i], COLOR_PAIR(i + 1));
        ui->panels[i] = new_panel(ui->windows[i]);
        box(ui->windows[i], 0, 0);
        mvwprintw(ui->windows[i], 0, 2, "Window %d", i);

        ui->sub_windows[i] = derwin(ui->windows[i], ROW - 2, COL / 3 - 2, 1, 1);
        wbkgd(ui->sub_windows[i], COLOR_PAIR(i + 1));
        ui->sub_panels[i] = new_panel(ui->sub_windows[i]);
        scrollok(ui->sub_windows[i], TRUE);

        hide_panel(ui->panels[i]);
        hide_panel(ui->sub_panels[i]);

        if (i == 0 || i == 1) {
            show_panel(ui->panels[i]);
            show_panel(ui->sub_panels[i]);
        }
    }
}

void create_progress_window(UI *ui, int width) {
    ui->progress_win = newwin(12, width, 0, 0);
    ui->progress_panel = new_panel(ui->progress_win);
    box(ui->progress_win, 0, 0);
    show_panel(ui->progress_panel);
}

void create_input_window(UI *ui, int COL, int ROW) {
    ui->input_win = newwin(3, COL / 3, ROW - 3, 0);
    box(ui->input_win, 0, 0);
    mvwprintw(ui->input_win, 0, 2, "Input:");
}

void create_input_form(UI *ui, int COL) {
    ui->field[0] = new_field(1, 11, 0, 0, 0, 0);
    ui->field[1] = NULL;
    set_max_field(ui->field[0], 10);
    set_field_back(ui->field[0], A_UNDERLINE);

    ui->form = new_form(ui->field);
    set_form_win(ui->form, ui->input_win);
    set_form_sub(ui->form, derwin(ui->input_win, 1, COL / 3 - 4, 1, 2));
    post_form(ui->form);
}

void init_ui(UI *ui, char progress[10], int *current_window, struct timespec *start_times, struct timespec *finish_times)
{
    init_ncurses();
    init_colours();

    int COL, ROW;
    getmaxyx(stdscr, ROW, COL);
    int width = COL / 3;

    create_windows(ui, COL, ROW);
    create_progress_window(ui, width);
    create_input_window(ui, COL, ROW);
    create_input_form(ui, COL);

    refresh();           // Refresh the standard screen
    wrefresh(ui->input_win); // Refresh the input box window

    display_progress_bars(ui->progress_win, progress, start_times, finish_times);

    wrefresh(ui->progress_win); // add this line to refresh the progress window

    *current_window = 0;
    update_panels();
    doupdate();
}

void end_ui(UI *ui)
{
    unpost_form(ui->form);
    free_form(ui->form);
    free_field(ui->field[0]);

    // Free the memory allocated to the panels and windows
    for (int i = 0; i < 10; i++)
    {
        del_panel(ui->panels[i]);
        del_panel(ui->sub_panels[i]);
        delwin(ui->sub_windows[i]);
        delwin(ui->windows[i]);
    }

    del_panel(ui->progress_panel);
    delwin(ui->input_win);
    delwin(ui->progress_win);

    endwin();
    free(ui);
}
