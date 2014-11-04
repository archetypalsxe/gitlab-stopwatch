#include "Timer.h"

void initTimer(TimerP timer) {
	timer->running = FALSE;
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

void startTimer(TimerP timer) {
	if(timer->running) {
		g_print("Timer already running\n");
	} else {
		timer->running = TRUE;
		g_print("Starting timer\n");
	}
}

void stopTimer(TimerP timer) {
	if(timer->running) {
		timer->running = FALSE;
		g_print ("Stopping timer\n");
	} else {
		g_print("Timer not running\n");
	}
}
