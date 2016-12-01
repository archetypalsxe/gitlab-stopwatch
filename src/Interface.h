#ifndef INTERFACE_H
#define INTERFACE_H

#include "Timer.h"

/**
 * This "class" holds all of the GtkWidgets that are necesary for making
 * the display work. It also includes a pointer to the Timer object so that
 * we can keep track of time
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

// Public function prototypes
void initializeObjects();

#endif
