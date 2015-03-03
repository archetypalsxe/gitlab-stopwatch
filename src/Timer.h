#ifndef _timer_h
#define _timer_h

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct Timer {
	gboolean running;
	struct tm *startLocalTime;
	struct tm *stopLocalTime;
	time_t startTime;
	time_t endTime;
	gchar elapsedTime[256];
	guint timeoutIdentifier;
}*TimerP;

void initTimer(TimerP);

void getElapsedTime(TimerP);
const gchar *getTime();
gboolean alertUser(gpointer[]);
gboolean startTimer(TimerP);
gboolean stopTimer(TimerP);

#endif
