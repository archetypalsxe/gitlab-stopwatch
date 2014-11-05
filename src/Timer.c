#include "Timer.h"

void initTimer(TimerP timer) {
	timer->running = FALSE;
}

void getElapsedTime(TimerP timer) {
	sprintf(timer->elapsedTime, "");

	int elapsedTime = (int)difftime(timer->endTime, timer->startTime);
	int numDays = elapsedTime / 60 / 60 / 24;
	elapsedTime -= numDays * 60 * 60 * 24;
	int numHours = elapsedTime / 60 / 60;
	elapsedTime -= numHours * 60 * 60;
	int numMinutes = elapsedTime / 60;
	elapsedTime -= numMinutes * 60;
	int numSeconds = elapsedTime;

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

		timer->startTime = time(NULL);
		timer->startLocalTime = localtime(&timer->startTime);

		return TRUE;
	}
}

gboolean stopTimer(TimerP timer) {
	if(timer->running) {
		timer->running = FALSE;

		timer->endTime = time(NULL);
		timer->stopLocalTime = localtime(&timer->endTime);

		return TRUE;
	} else {
		return FALSE;
	}
}
