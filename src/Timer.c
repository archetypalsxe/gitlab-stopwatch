#include "Timer.h"

/**
 *              PRIVATE FUNCTION PROTOTYPES
 */

/**
 * Alerts the user that the timer has not been running. Sets up a followup
 * alert based on the provided number of microseconds
 */
gboolean alertUser(TimerP);

/**
 * Called when the timer has been started. Done to remove duplciate code
 * between starting and resuming after pause
 */
gboolean timerStarted(TimerP);

/**
 * Called when the timer is stopped. Done to remove duplicate code between
 * pausing and stopping
 *
 * @param TimerP timer
 * @param gboolean paused Whether or not we are pausing (instead of stopping)
 */
gboolean timerStopped(TimerP, gboolean);
/**
 * Load up the elapsed time
 */
void loadElapsedTime(TimerP);
/**
 * Takes in the number of seconds that have elapsed to be formatted and
 * put into the provided string
 */
void setElapsedTime(int, char*);

gboolean alertUser(TimerP timer) {
    if(!timer->stopped && !timer->paused) {
        return FALSE;
    }
    NotifyNotification *notification;
    notify_init("Basic");

    notification = notify_notification_new(
        "Timer is not running", NULL, NULL
    );
    int alertTimeout = ALERT_TIMEOUT;
    notify_notification_set_timeout (notification, alertTimeout);
    notify_notification_show (notification, NULL);
    return TRUE;
}

gboolean isPaused(TimerP timer)
{
    return timer->paused;
}

gboolean isStopped(TimerP timer)
{
    return timer->stopped;
}

gboolean startTimer(TimerP timer) {
    return timerStarted(timer);
}

gboolean stopTimer(TimerP timer) {
    return timerStopped(timer, FALSE);
}

gboolean timerStarted (TimerP timer)
{
    if(!timer->stopped && !timer->paused) {
        return FALSE;
    }

    timer->startTime = time(NULL);

    // If we are resuming instead of starting
    if(!timer->paused) {
        timer->elapsedSeconds = 0;
    }

    timer->stopped = FALSE;
    timer->paused = FALSE;

    //Remove notification that timer is not running
    g_source_remove(timer->timeoutIdentifier);

    return TRUE;
}

gboolean timerStopped (TimerP timer, gboolean paused)
{
    if(timer->stopped) {
        return FALSE;
    }

    timer->endTime = time(NULL);

    if(!timer->paused) {
        loadElapsedTime(timer);
    }

    timer->paused = paused;

    if(!paused) {
        timer->stopped = TRUE;
    }


    /* Set up notification for every 5 minutes (300000) */
    timer->timeoutIdentifier = g_timeout_add_seconds(
        timer->alertFrequency,
        (GSourceFunc)alertUser,
        timer
    );

    return TRUE;
}

void debug(TimerP timer)
{
    printf("***Debugging***\n");
    printf("%p\n", timer);
    printf("Stopped: %d\n", timer->stopped);

    char buff[20];
    printf("Start Time: %s", ctime(&(timer->startTime)));
    printf("End Time: %s", ctime(&(timer->endTime)));
    printf("Elapsed Time: %s\n", timer->elapsedTime);
    printf("Timeout Identifier: %d\n", timer->timeoutIdentifier);
    printf("Elapsed Seconds: %d\n", timer->elapsedSeconds);
    printf("Paused Status: %d\n", timer->paused);

    printf("***End of Debugging***\n\n");
}

void initTimer(TimerP timer)
{
    timer->stopped = TRUE;
    timer->alertFrequency = ALERT_FREQUENCY;
    timer->timeoutIdentifier = g_timeout_add_seconds(
        timer->alertFrequency,
        (GSourceFunc)alertUser,
        timer
    );
    timer->elapsedSeconds = 0;
}

void loadCurrentLocalTime(char * string, int length)
{
    loadProvidedLocalTime(time(NULL), string, length);
}

void loadCurrentElapsedTime(char* string, TimerP timer)
{
    int elapsedTime =
        timer->elapsedSeconds +
        (int)difftime(time(NULL), timer->startTime);
    setElapsedTime(elapsedTime, string);
}

void loadProvidedLocalTime(time_t time, char * string, int length)
{
    struct tm *localTime;
    localTime = localtime(&time);
    strftime(string, length, "%I:%M:%S%P", localTime);
}

void loadStartLocalTime(TimerP timer, char * string, int length)
{
    loadProvidedLocalTime(timer->startTime, string, length);
}

void loadStopLocalTime(TimerP timer, char * string, int length)
{
    loadProvidedLocalTime(timer->endTime, string, length);
}

/**
 * @TODO Need to better handle elapsed time
 */
void loadElapsedTime(TimerP timer)
{
    int elapsedTime =
        timer->elapsedSeconds +
        (int)difftime(time(NULL), timer->startTime);

    // @TODO Remove this?
    timer->elapsedSeconds = elapsedTime;
    setElapsedTime(elapsedTime, timer->elapsedTime);
}

void pauseTimer(TimerP timer)
{
    timerStopped(timer, TRUE);
}

void resumeTimer(TimerP timer)
{
    timerStarted(timer);
}

void setElapsedTime(int seconds, char *string)
{
    sprintf(string, "");
    int numDays = seconds / 60 / 60 / 24;
    seconds -= numDays * 60 * 60 * 24;
    int numHours = seconds / 60 / 60;
    seconds -= numHours * 60 * 60;
    int numMinutes = seconds / 60;
    seconds -= numMinutes * 60;
    int numSeconds = seconds;

    if(numDays > 0) {
        sprintf(string, "%d days", numDays);
    }

    if(numHours > 0) {
        if(strnlen(string, sizeof(string)) > 0) {
            sprintf(string, "%s %d hours", string, numHours);
        } else {
            sprintf(string, "%d hours", numHours);
        }
    }

    if(numMinutes > 0) {
        if(strnlen(string, sizeof(string)) > 0) {
            sprintf(string, "%s %d minutes", string, numMinutes);
        } else {
            sprintf(string, "%d minutes", numMinutes);
        }
    }

    if(numSeconds > 0) {
        if(strnlen(string, sizeof(string)) > 0) {
            sprintf(string, "%s %d seconds", string, numSeconds);
        } else {
            sprintf(string, "%d seconds", numSeconds);
        }
    }

    if(strnlen(string, sizeof(string)) == 0) {
        sprintf(string, "%d seconds", 0);
    }
}

