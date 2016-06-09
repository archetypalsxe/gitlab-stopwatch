#ifndef _timer_h
#define _timer_h

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <libnotify/notify.h>
#include <gtk/gtk.h>

typedef struct Timer {
	gboolean running;
	struct tm *startLocalTime;
	struct tm *stopLocalTime;
	time_t startTime;
	time_t endTime;
	gchar elapsedTime[256];
	guint timeoutIdentifier;
}*TimerP;

typedef struct TimerData {
    TimerP timerPointer;
    GtkWidget *grid;
    GtkWidget *window;
    GtkWidget *startButton;
    GtkWidget *stopButton;
	GtkWidget *lapButton;
} *TimerDataP;

void debug(TimerP);
void initTimer(TimerP);

void setElapsedTime(int, TimerP);
void getElapsedTime(TimerP);
const gchar *getTime();
gboolean alertUser(TimerP);
gboolean startTimer(TimerP);
gboolean stopTimer(TimerP);

#endif
