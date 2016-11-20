#include "Timer.h"

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

void displayWorkingRequest (
    gchar time[256],
    GtkWidget *grid,
    GtkWidget *window,
    TimerDataP timerData
) {
    GtkWidget *dialog;
    GtkWidget *textEntry;
    GtkWidget *label;

    label = gtk_label_new("What Are You Working On?");

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons(
        "Starting Timer",
        GTK_WINDOW(window),
        flags,
        NULL
    );

    textEntry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(textEntry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

    gtk_box_pack_start(
        GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
        label,
        FALSE,
        FALSE,
        0
    );
    gtk_dialog_add_action_widget(GTK_DIALOG(dialog), textEntry, 1);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));

    updateStartTime(
        time,
        grid,
        window,
        gtk_entry_get_text(GTK_ENTRY(textEntry)),
        timerData
    );
    gtk_widget_destroy(dialog);
}

/**
 * Initial setup of the timer (constructor). Sets that we are not running
 * and also sets a notification
 */
void initTimer(TimerP timer) {
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

/**
 * Get the time that has elapsed thus far
 *
 * @param TimerP timer
 * @return gchar
 */
gchar * getCurrentTime(TimerP timer) {
    loadCurrentTime(timer);
    return timer->elapsedTime;
}

/**
 * Convert a provided number of seconds into a user friendly display
 *
 * @param int seconds
 */
void setElapsedTime(int seconds, TimerP timer) {
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

void getElapsedTime(TimerP timer) {
	sprintf(timer->elapsedTime, "");

	setElapsedTime(timer->elapsedSeconds, timer);
}

void loadCurrentTime(TimerP timer) {
    sprintf(timer->elapsedTime, "");
    int elapsedTime =
        timer->elapsedSeconds +
        (int)difftime(time(NULL), timer->startTime);
    setElapsedTime(elapsedTime, timer);
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

const gchar *getTime() {
	time_t currentTime;
	struct tm *localTime;
	gchar *buffer[256];

	currentTime = time(NULL);
	localTime = localtime(&currentTime);
	strftime(*buffer, 256, "%I:%M:%S%P", localTime);
	return *buffer;
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
            timer->elapsedSeconds += (int)difftime(
                timer->endTime,
                timer->startTime
            );
        }

		/* Set up notification for every 5 minutes (300000) */
		timer->timeoutIdentifier = g_timeout_add(200000, (GSourceFunc)alertUser, timer);

		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * Called to pause the timer
 *
 * @param TimerP timer
 */
void pauseTimer(TimerP timer)
{
    // We are already paused
    if (timer->paused) {
        return;
    }
    timer->endTime = time(NULL);
    timer->stopLocalTime = localtime(&timer->endTime);
    timer->elapsedSeconds += (int)difftime(
        timer->endTime,
        timer->startTime
    );
    timer->timeoutIdentifier = g_timeout_add(
        200000,
        (GSourceFunc)alertUser, timer
    );

    timer->paused = TRUE;
    getElapsedTime(timer);
}

/**
 * Called to resume the timer
 *
 * @param TimerP timer
 */
void resumeTimer(TimerP timer)
{
    // Already not paused
    if (!timer->paused) {
        return;
    }
    timer->paused = FALSE;
	timer->startTime = time(NULL);
	timer->startLocalTime = localtime(&timer->startTime);
	g_source_remove(timer->timeoutIdentifier);

}

void startTimerPressed (GtkWidget *widget, TimerDataP timerData)
{
    TimerP timer = timerData->timerPointer;

    gboolean success = startTimer(timer);
    if(success) {
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = timerData->grid;
        GtkWidget *window = timerData->window;
        GtkWidget *startButton = timerData->startButton;
        GtkWidget *stopButton = timerData->stopButton;
        GtkWidget *lapButton = timerData->lapButton;
        GtkWidget *pauseButton = timerData->pauseButton;

        gtk_button_set_label(GTK_BUTTON(pauseButton), "Pause");

        strftime(buffer, 256, "%I:%M:%S%P", timer->startLocalTime);
        windowTime = gtk_label_new(buffer);
        windowAction = gtk_label_new("Timer Started");
        windowElapsed = gtk_label_new("--");
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_hide(startButton);
        gtk_widget_show(pauseButton);
        gtk_widget_show(lapButton);
        gtk_widget_show(stopButton);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);
        displayWorkingRequest(buffer, grid, window, timerData);
    }
}

void updateStartTime(
    gchar time[256],
    GtkWidget *grid,
    GtkWidget *window,
    const gchar *text,
    TimerDataP timerData
) {
    GtkWidget *windowTime;
    GtkWidget *windowAction;
    GtkWidget *windowElapsed;

    windowTime = gtk_label_new(time);
    windowAction = gtk_label_new("Timer Started");
    strlen(text);

    TimerP timer = timerData->timerPointer;
    sprintf(timer->subject, "%s", text);

    windowElapsed = gtk_label_new(text);
    gtk_grid_remove_row(GTK_GRID(grid), 0);
    gtk_grid_insert_row(GTK_GRID(grid), 0);
    gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);

    gtk_widget_show(windowTime);
    gtk_widget_show(windowAction);
    gtk_widget_show(windowElapsed);
}
