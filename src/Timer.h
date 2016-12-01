#ifndef TIMER_H
#define TIMER_H

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <libnotify/notify.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#define WINDOW_WIDTH 550
#define WINDOW_HEIGHT 200

/**
 * The "class" that has all the data for the timer itself and handles all
 * the user interaction
 *
 * @TODO Make this just deal with the timer itself and not anything to do
 * with the UI (shouldn't have subject and stuff)
 */
typedef struct Timer {
    gboolean running;
    gboolean paused;
    struct tm *startLocalTime;
    struct tm *stopLocalTime;
    time_t startTime;
    time_t endTime;
    gchar elapsedTime[256];
    gchar subject[256];
    // Used for notifying user that we are not running
    guint timeoutIdentifier;
    int elapsedSeconds;
}*TimerP;

void debug(TimerP);
/**
 * Initial setup of the timer (constructor). Sets that we are not running
 * and also sets a notification
 */
void initTimer(TimerP);
void pauseTimer(TimerP);
void resumeTimer(TimerP);
/**
 * Returns the time that has elapsed thus far
 */
gchar *getCurrentTime(TimerP);
GtkWidget* createGrid();
GtkWidget* createWindow();
/**
 * Convert a provided number of seconds into a user friendly display
 */
void setElapsedTime(int, TimerP);
void loadCurrentTime(TimerP);
void getElapsedTime(TimerP);
const gchar *getTime();
gboolean alertUser(TimerP);
gboolean startTimer(TimerP);
gboolean stopTimer(TimerP);

#endif
