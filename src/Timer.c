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
}

void stopTimer(TimerP timer) {
}
