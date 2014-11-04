#include "Timer.h"

void initTimer(TimerP timer) {
	timer->running = FALSE;
}

const gchar *getElapsedTime(TimerP timer) {
	char returnString[256];
	const gchar *buffer[256];

	int elapsedTime = (int)difftime(timer->endTime, timer->startTime);
	int numDays = elapsedTime / 60 / 60 / 24;
	elapsedTime -= numDays * 60 * 60 * 24;
	int numHours = elapsedTime / 60 / 60;
	elapsedTime -= numHours * 60 * 60;
	int numMinutes = elapsedTime / 60;
	elapsedTime -= numMinutes * 60;
	int numSeconds = elapsedTime;

	if(numDays > 0) {
		sprintf(returnString, "%d days", numDays);
	}

	if(numHours > 0) {
		if(strnlen(returnString, sizeof(returnString)) > 0) {
			sprintf(returnString, "%s %d hours", returnString, numHours);
		} else {
			sprintf(returnString, "%d hours", numHours);
		}
	}

	if(numMinutes > 0) {
		if(strnlen(returnString, sizeof(returnString)) > 0) {
			sprintf(returnString, "%s %d minutes", returnString, numMinutes);
		} else {
			sprintf(returnString, "%d minutes", numMinutes);
		}
	}

	if(numSeconds > 0) {
		if(strnlen(returnString, sizeof(returnString)) > 0) {
			sprintf(returnString, "%s %d seconds", returnString, numSeconds);
		} else {
			sprintf(returnString, "%d seconds", numSeconds);
		}
	}

	g_print("\nDiff: %d\n", elapsedTime);
	g_print("Num Days: %d\n", numDays);
	g_print("Num Hours: %d\n", numHours);
	g_print("Num Minutes: %d\n", numMinutes);
	g_print("Num Seconds: %d\n", numSeconds);
	*buffer = returnString;
	g_print("Return String: %s\n\n", *buffer);
	return *buffer;
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
		g_print("Timer already running\n");
		return FALSE;
	} else {
		timer->running = TRUE;
		g_print("Starting timer\n");

		timer->startTime = time(NULL);
		timer->startLocalTime = localtime(&timer->startTime);

		return TRUE;
	}
}

gboolean stopTimer(TimerP timer) {
	if(timer->running) {
		timer->running = FALSE;
		g_print ("Stopping timer\n");

		timer->endTime = time(NULL);
		timer->stopLocalTime = localtime(&timer->endTime);

		return TRUE;
	} else {
		g_print("Timer not running\n");
		return FALSE;
	}
}
