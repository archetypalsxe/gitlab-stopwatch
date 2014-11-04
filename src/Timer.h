#ifndef _timer_h
#define _timer_h

#include <glib.h>
#include <string.h>
#include <stdio.h>

typedef struct Timer {
	gboolean running;
	struct tm *startLocalTime;
	struct tm *stopLocalTime;
	time_t startTime;
	time_t endTime;
}*TimerP;

void initTimer(TimerP);

const gchar *getElapsedTime(TimerP);
const gchar *getTime();
gboolean startTimer(TimerP);
gboolean stopTimer(TimerP);

#endif
