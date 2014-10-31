#ifndef _timer_h
#define _timer_h

#include <glib.h>

typedef struct Timer {
	gboolean running;
	struct tm *startTime;
}*TimerP;

void initTimer(TimerP);

const gchar *getTime();
void startTimer(TimerP);
void stopTimer(TimerP);

#endif
