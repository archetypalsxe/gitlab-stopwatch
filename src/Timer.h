#ifndef TIMER_H
#define TIMER_H

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <libnotify/notify.h>
#include <gtk/gtk.h>
#include <stdlib.h>

/**
 * Frequency is in seconds
 */
#define ALERT_FREQUENCY 180;
/**
 * Is in milliseconds
 */
#define ALERT_TIMEOUT 8000;

/**
 * The "class" that has all the data for the timer itself and handles all
 * the user interaction
 */
typedef struct Timer {
    gboolean stopped;
    gboolean paused;
    time_t startTime;
    time_t endTime;
    gchar elapsedTime[256];
    gchar subject[256];
    // Used for notifying user that we are not running
    guint timeoutIdentifier;
    /**
     * The frequency we will alert the user in micro seconds
     */
    int alertFrequency;
    int elapsedSeconds;
}*TimerP;


/**
 *          PUBLIC FUNCTION PROTOTYPES
 */
/**
 * Returns whether or not the timer is paused
 */
gboolean isPaused(TimerP);
/**
 * Returns whether or not the timer is stopped
 */
gboolean isStopped(TimerP);
/**
 * Start the timer. Returns whether the starting was successful or not
 */
gboolean startTimer(TimerP);
/**
 * Stop the timer. Returns whether or not the stopping was successful
 */
gboolean stopTimer(TimerP);
/**
 * Returns the current time that has elapsed
 */
gchar *getElapsedTime(TimerP);
/**
 * Used for outputting debugging information to the console
 *
 * @TODO This should be able to be initiated in the calling parameters
 */
void debug(TimerP);
/**
 * Initial setup of the timer (constructor). Sets that we are not running
 * and also sets a notification
 */
void initTimer(TimerP);
/**
 * Load the current local time into the provided string
 */
void loadCurrentLocalTime(char*, int);
/**
 * Load the provided local time into the provided string
 */
void loadProvidedLocalTime(time_t, char*, int);
/**
 * Load the start time from the provided timer into the provided string
 */
void loadStartLocalTime(TimerP, char*, int);
/**
 * Load the stop time from the provided timer into the provided string
 */
void loadStopLocalTime(TimerP, char*, int);
/**
 * Pause the timer so it is no longer running but able to be easily resumed
 */
void pauseTimer(TimerP);
/**
 * Resume the timer after it has been paused
 */
void resumeTimer(TimerP);

#endif
