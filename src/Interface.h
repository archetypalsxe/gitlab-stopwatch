#ifndef INTERFACE_H
#define INTERFACE_H

#include "Timer.h"

/**
 * This "class" holds all of the GtkWidgets that are necesary for making
 * the display work. It also includes a pointer to the Timer object so that
 * we can keep track of time
 *
 * @TODO Make this class just deal with the user interface, this class
 * will get bigger and the other (Timer) will get smaller
 */
typedef struct Interface {
    TimerP timerPointer;
    GtkWidget *grid;
    GtkWidget *window;
    GtkWidget *startButton;
    GtkWidget *stopButton;
    GtkWidget *lapButton;
    GtkWidget *pauseButton;
} *InterfacePointer;

void displayWorkingRequest(
    gchar[256], GtkWidget*, GtkWidget*, InterfacePointer
);
void initializeObjects();
void lapButtonPressed (GtkWidget*, InterfacePointer);
void pauseButtonPressed (GtkWidget*, InterfacePointer);
void startTimerPressed(GtkWidget*, InterfacePointer);
void stopTimerPressed(GtkWidget*, InterfacePointer);
void updateStartTime(
    char[256], GtkWidget*, GtkWidget*, const gchar*, InterfacePointer
);

#endif
