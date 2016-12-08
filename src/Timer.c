#include "Timer.h"

// Private function prototypes
/**
 * Returns a formatted string of the current time
 */
const gchar *getCurrentTime();
/**
 * Alerts the user that the timer has not been running. Sets up a followup
 * alert based on the provided number of microseconds
 *
 * @TODO Ability to pass in number of seconds until next alert
 */
gboolean alertUser(TimerP);
/**
 * Load up the elapsed time
 */
void loadElapsedTime(TimerP);
void setElapsedTime(int, TimerP);

const gchar *getCurrentTime() {
    time_t currentTime;
    struct tm *localTime;
    gchar *buffer[256];

    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    strftime(*buffer, 256, "%I:%M:%S%P", localTime);
    return *buffer;
}


gboolean alertUser(TimerP timer) {
    if(timer->running) {
        return FALSE;
    }
    NotifyNotification *notification;
    notify_init("Basic");

    notification = notify_notification_new("Timer is not running", NULL, NULL);
    notify_notification_set_timeout (notification, 8000);
    notify_notification_show (notification, NULL);
    return TRUE;
}

gboolean startTimer(TimerP timer) {
    if(timer->running) {
        return FALSE;
    } else {
        timer->running = TRUE;
        timer->paused = FALSE;

        timer->startTime = time(NULL);
        timer->elapsedSeconds = 0;
        timer->startLocalTime = localtime(&timer->startTime);

        //Remove notification that timer is not running
        g_source_remove(timer->timeoutIdentifier);

        return TRUE;
    }
}

gboolean stopTimer(TimerP timer) {
    if(timer->running) {
        timer->running = FALSE;
        timer->endTime = time(NULL);
        timer->stopLocalTime = localtime(&timer->endTime);

        if(timer->paused) {
            timer->paused = FALSE;
        } else {
            loadElapsedTime(timer);
        }


        /* Set up notification for every 5 minutes (300000) */
        timer->timeoutIdentifier = g_timeout_add(
            200000,
            (GSourceFunc)alertUser,
            timer
        );

        return TRUE;
    } else {
        return FALSE;
    }
}


gchar * getElapsedTime(TimerP timer)
{
    loadElapsedTime(timer);
    return timer->elapsedTime;
}

void debug(TimerP timer)
{
    printf("***Debugging***\n");
    printf("%p\n", timer);
    printf("Running: %d\n", timer->running);

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
    timer->running = FALSE;
    /**
     * Set up notifications. 5 Minutes is 300000
     */
    timer->timeoutIdentifier = g_timeout_add(
        200000,
        (GSourceFunc)alertUser,
        timer
    );
    timer->elapsedSeconds = 0;
}

void loadElapsedTime(TimerP timer)
{
    sprintf(timer->elapsedTime, "");
    int elapsedTime =
        timer->elapsedSeconds +
        (int)difftime(time(NULL), timer->startTime);

    timer->elapsedSeconds = elapsedTime;
    setElapsedTime(elapsedTime, timer);
}

void pauseTimer(TimerP timer)
{
    // We are already paused
    if (timer->paused) {
        return;
    }
    timer->endTime = time(NULL);
    timer->stopLocalTime = localtime(&timer->endTime);
    timer->timeoutIdentifier = g_timeout_add(
        200000,
        (GSourceFunc)alertUser, timer
    );

    timer->paused = TRUE;
    loadElapsedTime(timer);
}

void resumeTimer(TimerP timer)
{
    // Not currently paused
    if (!timer->paused) {
        return;
    }
    timer->paused = FALSE;
    timer->startTime = time(NULL);
    timer->startLocalTime = localtime(&timer->startTime);
    g_source_remove(timer->timeoutIdentifier);
}

void setElapsedTime(int seconds, TimerP timer)
{
    int numDays = seconds / 60 / 60 / 24;
    seconds -= numDays * 60 * 60 * 24;
    int numHours = seconds / 60 / 60;
    seconds -= numHours * 60 * 60;
    int numMinutes = seconds / 60;
    seconds -= numMinutes * 60;
    int numSeconds = seconds;

    if(numDays > 0) {
        sprintf(timer->elapsedTime, "%d days", numDays);
    }

    if(numHours > 0) {
        if(strnlen(timer->elapsedTime, sizeof(timer->elapsedTime)) > 0) {
            sprintf(timer->elapsedTime, "%s %d hours", timer->elapsedTime, numHours);
        } else {
            sprintf(timer->elapsedTime, "%d hours", numHours);
        }
    }

    if(numMinutes > 0) {
        if(strnlen(timer->elapsedTime, sizeof(timer->elapsedTime)) > 0) {
            sprintf(timer->elapsedTime, "%s %d minutes", timer->elapsedTime, numMinutes);
        } else {
            sprintf(timer->elapsedTime, "%d minutes", numMinutes);
        }
    }

    if(numSeconds > 0) {
        if(strnlen(timer->elapsedTime, sizeof(timer->elapsedTime)) > 0) {
            sprintf(timer->elapsedTime, "%s %d seconds", timer->elapsedTime, numSeconds);
        } else {
            sprintf(timer->elapsedTime, "%d seconds", numSeconds);
        }
    }

    if(strnlen(timer->elapsedTime, sizeof(timer->elapsedTime)) == 0) {
        sprintf(timer->elapsedTime, "%d seconds", 0);
    }
}

