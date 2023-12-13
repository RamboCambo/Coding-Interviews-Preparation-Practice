#include <ncurses.h>
#include <form.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "ui.h"
#include "queue.h"
#include "shared_memory.h"

// Create queue object
Queue *queue;

// Create a mutex to protect the queue
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *sem;
SyncronisedSharedFields *synced_fields;
UnsyncronisedSharedFields *unsynced_fields;
UI *ui;

struct timespec start_times[NUM_SLOTS];
struct timespec finish_times[NUM_SLOTS];

// Thread to monitor the queue
void *monitor_queue()
{
    while (1)
    {
        pthread_mutex_lock(&queue_mutex);
        // If the queue is empty, sleep for 1 second
        if (queue->front == NULL)
        {
            // Unlock the mutex
            pthread_mutex_unlock(&queue_mutex);
            usleep(100000); // Sleep for 100ms
            continue;
        }
    
        uint32_t num = dequeue(queue);
        pthread_mutex_unlock(&queue_mutex);

        sem_wait(sem);
        synced_fields->number = num;
        synced_fields->clientflag = 1;
        sem_post(sem);

        int waiting_for_server = 1;
        while(waiting_for_server)
        {
            sem_wait(sem);
            waiting_for_server = synced_fields->clientflag;
            sem_post(sem);
            // usleep(100000); // Sleep for 100ms
        }

        sem_wait(sem);
        uint32_t request_slot = synced_fields->number;
        sem_post(sem);

        // Unlock the mutex
        if(request_slot == NUM_SLOTS)
        {
            pthread_mutex_lock(&ui_mutex);
            mvwprintw(ui->input_win, 1, 15, "Server busy! 10 reqs already!");
            wrefresh(ui->input_win);
            pthread_mutex_unlock(&ui_mutex);
            usleep(100000);
            continue;
        }

        if(num == 0)
        {
            pthread_mutex_lock(&ui_mutex);
            mvwprintw(ui->input_win, 1, 15, "Test mode activated!          ");
            wrefresh(ui->input_win);
            pthread_mutex_unlock(&ui_mutex);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &start_times[request_slot]);
            clock_gettime(CLOCK_REALTIME, &finish_times[request_slot]);

            pthread_mutex_lock(&ui_mutex);
            mvwprintw(ui->input_win, 1, 15, "Processing now on %u!         ", request_slot);
            wrefresh(ui->input_win);
            pthread_mutex_unlock(&ui_mutex);
        }

        // Sleep for 100ms
        // usleep(100000);
    }
}

// Thread to monitor the progress array
void *monitor_progress()
{
    while (1)
    {
        pthread_mutex_lock(&ui_mutex);
        display_progress_bars(ui->progress_win, unsynced_fields->progress, start_times, finish_times);
        pthread_mutex_unlock(&ui_mutex);

        // Sleep for 100ms
        // usleep(100000);
    }
}

// Thread to monitor the slots
void *monitor_slots()
{
    while (1)
    {
        sem_wait(sem);
        uint32_t slot[NUM_SLOTS];
        char serverflag[NUM_SLOTS];
        memcpy(slot, synced_fields->slot, NUM_SLOTS * sizeof(uint32_t));
        memcpy(serverflag, synced_fields->serverflag, NUM_SLOTS);

        // Mark serverflag as 0 to indicate that the client has read the data
        memset(synced_fields->serverflag, 0, NUM_SLOTS);
        sem_post(sem);

        pthread_mutex_lock(&ui_mutex);
        display_slots(ui->sub_windows, slot, serverflag);
        pthread_mutex_unlock(&ui_mutex);

        // Sleep for 10ms
        // usleep(10000);
    }
}

int main()
{
    for(int i = 0; i < NUM_SLOTS; i++)
    {
        clock_gettime(CLOCK_REALTIME, &start_times[i]);
        clock_gettime(CLOCK_REALTIME, &finish_times[i]);
    }

    int current_window = 0;
    int digit_count = 0;

    int fd1;
    int fd2;
    void *syncronised_shared_mem;
    void *unsyncronised_shared_mem;

    if (access_shared_memory(&fd1, &fd2, &syncronised_shared_mem, &unsyncronised_shared_mem, &sem) == -1)
    {
        return -1;
    }

    synced_fields = (SyncronisedSharedFields *)syncronised_shared_mem;
    unsynced_fields = (UnsyncronisedSharedFields *)unsyncronised_shared_mem;

    ui = malloc(sizeof(UI));
    init_ui(ui, unsynced_fields->progress, &current_window, start_times, finish_times);

    // Create queue object
    queue = createQueue();

    // Create a thread to monitor the queue
    pthread_t queue_thread;
    pthread_create(&queue_thread, NULL, monitor_queue, queue);

    // Create a thread to monitor the progress array
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_progress, NULL);

    // Create a thread to monitor the slots
    pthread_t slot_thread;
    pthread_create(&slot_thread, NULL, monitor_slots, NULL);

    uint64_t value = 0;

    while (1)
    {
        int ch = getch();

        pthread_mutex_lock(&ui_mutex);
        if (ch == KEY_LEFT)
        {
            hide_panel(ui->panels[current_window]);
            hide_panel(ui->panels[current_window + 1]);
            hide_panel(ui->sub_panels[current_window]);
            hide_panel(ui->sub_panels[current_window + 1]);
            current_window -= 2;

            if (current_window < 0)
            {
                current_window = 8;
            }

            show_panel(ui->panels[current_window]);
            show_panel(ui->panels[current_window + 1]);
            show_panel(ui->sub_panels[current_window]);
            show_panel(ui->sub_panels[current_window + 1]);
        }
        else if (ch == KEY_RIGHT)
        {
            hide_panel(ui->panels[current_window]);
            hide_panel(ui->panels[current_window + 1]);
            hide_panel(ui->sub_panels[current_window]);
            hide_panel(ui->sub_panels[current_window + 1]);
            current_window += 2;

            if (current_window > 8)
            {
                current_window = 0;
            }

            show_panel(ui->panels[current_window]);
            show_panel(ui->panels[current_window + 1]);
            show_panel(ui->sub_panels[current_window]);
            show_panel(ui->sub_panels[current_window + 1]);            
        }
        else if (ch == 'q' || ch == 'Q')
        {
            break;
        }
        else if (digit_count > 0 && (ch == KEY_BACKSPACE || ch == '\b' || ch == 127))
        { 
            // Only process backspace if there's actual content
            form_driver(ui->form, REQ_DEL_PREV);
            digit_count--;

            // Update the value of the number
            value /= 10;
        }
        else if (digit_count < 10 && ch >= '0' && ch <= '9')
        {
            form_driver(ui->form, ch);
            digit_count++;

            // Update the value of the number
            value *= 10;
            value += ch - '0';
        }
        // Enter pressed
        else if (ch == '\n')
        {
            if (digit_count == 0)
            {
                mvwprintw(ui->input_win, 1, 15, "Input empty, try again!       ");
            }
            else if(value <= UINT32_MAX)
            {
                // Extra spaces to overwrite previous message
                mvwprintw(ui->input_win, 1, 15, "Success! Queueing number!     ");
                form_driver(ui->form, REQ_CLR_FIELD);

                uint32_t num = (uint32_t) value;

                value = 0;
                digit_count = 0;

                // Add number to queue
                // Use the mutex to protect the queue
                pthread_mutex_lock(&queue_mutex);
                enqueue(queue, num);
                pthread_mutex_unlock(&queue_mutex);
            }
            else 
            {
                mvwprintw(ui->input_win, 1, 15, "Input > 4294967295, try again!");
            }
        }

        update_panels();
        doupdate();
        wrefresh(ui->input_win); // Refresh the form window to bring it to the forefront

        pthread_mutex_unlock(&ui_mutex);

        // usleep(10000); // Sleep for 10ms
    }

    // cancel the thread
    pthread_cancel(queue_thread);
    pthread_cancel(monitor_thread);
    pthread_cancel(slot_thread);

    // delete the mutex
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&ui_mutex);

    // delete the queue
    free(queue);

    // delete the semaphore
    // sem_close(sem);

    // free(synced_fields);
    // free(unsynced_fields);
    end_ui(ui);
    return 0;
}