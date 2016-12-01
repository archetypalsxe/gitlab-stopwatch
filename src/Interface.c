#include "Interface.h"

void displayWorkingRequest (
    gchar time[256],
    GtkWidget *grid,
    GtkWidget *window,
    InterfacePointer timerData
) {
    GtkWidget *dialog;
    GtkWidget *textEntry;
    GtkWidget *label;

    label = gtk_label_new("What Are You Working On?");

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons(
        "Starting Timer",
        GTK_WINDOW(window),
        flags,
        NULL
    );

    textEntry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(textEntry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

    gtk_box_pack_start(
        GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
        label,
        FALSE,
        FALSE,
        0
    );
    gtk_dialog_add_action_widget(GTK_DIALOG(dialog), textEntry, 1);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));

    updateStartTime(
        time,
        grid,
        window,
        gtk_entry_get_text(GTK_ENTRY(textEntry)),
        timerData
    );
    gtk_widget_destroy(dialog);
}

void initializeObjects()
{
    GtkWidget *windowTime, *windowAction, *windowElapsed;
    TimerP timerPointer;
    timerPointer = malloc(sizeof(struct Timer));
    initTimer(timerPointer);

    GtkWidget *window = createWindow();
    GtkWidget *grid = createGrid();

    /* Pack the container in the window */
    gtk_container_add (GTK_CONTAINER (window), grid);

    GtkWidget *startButton = gtk_button_new_with_label ("Start");
    GtkWidget *stopButton = gtk_button_new_with_label ("Stop");
    GtkWidget *lapButton = gtk_button_new_with_label ("Display Time");
    GtkWidget *pauseButton = gtk_button_new_with_label ("Pause");

    InterfacePointer interfacePointer;
    interfacePointer = malloc(sizeof(struct Interface));
    interfacePointer->timerPointer = timerPointer;
    interfacePointer->grid = grid;
    interfacePointer->window = window;
    interfacePointer->startButton = startButton;
    interfacePointer->stopButton = stopButton;
    interfacePointer->lapButton = lapButton;
    interfacePointer->pauseButton = pauseButton;

    g_signal_connect (
        startButton,
        "clicked",
        G_CALLBACK (startTimerPressed),
        interfacePointer
    );
    g_signal_connect (
        stopButton,
        "clicked",
        G_CALLBACK (stopTimerPressed),
        interfacePointer
    );
    g_signal_connect (
        lapButton,
        "clicked",
        G_CALLBACK(lapButtonPressed),
        interfacePointer
    );
    g_signal_connect (
        pauseButton,
        "clicked",
        G_CALLBACK(pauseButtonPressed),
        interfacePointer
    );

    gtk_grid_attach (GTK_GRID (grid), startButton, 0, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), pauseButton, 0, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), lapButton, 2, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), stopButton, 1, 4, 1, 1);

    gtk_widget_show_all(window);
    gtk_widget_hide(stopButton);
    gtk_widget_hide(lapButton);
    gtk_widget_hide(pauseButton);
}

void lapButtonPressed (GtkWidget *widget, InterfacePointer timerData)
{
    gchar eventTimeString[256];
    gchar *currentElapsed;
    time_t currentTime;

    TimerP timer = timerData->timerPointer;
    currentElapsed = getCurrentTime(timer);

    GtkWidget *windowTime;
    GtkWidget *windowAction;
    GtkWidget *windowElapsed;
    GtkWidget *grid = timerData->grid;
    GtkWidget *window = timerData->window;

    currentTime = time(NULL);
    strftime(eventTimeString, 256, "%I:%M:%S%P", localtime(&currentTime));
    windowTime = gtk_label_new(eventTimeString);

    gchar actionString[256];
    sprintf(actionString, "Displaying Time (%s)", timer->subject);
    windowAction = gtk_label_new(actionString);

    windowElapsed = gtk_label_new(currentElapsed);
    gtk_grid_remove_row(GTK_GRID(grid), 3);
    gtk_grid_insert_row(GTK_GRID(grid), 0);
    gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
    gtk_widget_show(windowTime);
    gtk_widget_show(windowAction);
    gtk_widget_show(windowElapsed);
}

void pauseButtonPressed (GtkWidget *widget, InterfacePointer timerData)
{
    // Timer isn't running, pause button shouldn't be visible
    if (!timerData->timerPointer->running) {
        gtk_widget_hide(timerData->pauseButton);
    }

    if (!timerData->timerPointer->paused) {
        pauseTimer(timerData->timerPointer);

        gtk_widget_hide(timerData->lapButton);

        TimerP timer = timerData->timerPointer;
        gchar buffer[256];
        strftime(buffer, 256, "%I:%M:%S%P", timer->stopLocalTime);
        GtkWidget *windowTime;
        windowTime = gtk_label_new(buffer);

        gchar actionString[256];
        sprintf(actionString, "Timer paused (%s)", timer->subject);
        GtkWidget *windowAction;
        windowAction = gtk_label_new(actionString);

        GtkWidget *windowElapsed;
        windowElapsed = gtk_label_new(timer->elapsedTime);
        GtkWidget *grid = timerData->grid;
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);

        gtk_button_set_label(GTK_BUTTON(timerData->pauseButton), "Resume");
    } else {
        resumeTimer(timerData->timerPointer);
        gtk_widget_show(timerData->lapButton);

        TimerP timer = timerData->timerPointer;
        gchar buffer[256];
        strftime(buffer, 256, "%I:%M:%S%P", timer->startLocalTime);
        GtkWidget *windowTime;
        windowTime = gtk_label_new(buffer);

        gchar actionString[256];
        sprintf(actionString, "Timer resumed (%s)", timer->subject);
        GtkWidget *windowAction;
        windowAction = gtk_label_new(actionString);

        GtkWidget *windowElapsed;
        windowElapsed = gtk_label_new(timer->elapsedTime);
        GtkWidget *grid = timerData->grid;
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);


        gtk_button_set_label(GTK_BUTTON(timerData->pauseButton), "Pause");
    }
}

void startTimerPressed (GtkWidget *widget, InterfacePointer timerData)
{
    TimerP timer = timerData->timerPointer;

    gboolean success = startTimer(timer);
    if(success) {
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = timerData->grid;
        GtkWidget *window = timerData->window;
        GtkWidget *startButton = timerData->startButton;
        GtkWidget *stopButton = timerData->stopButton;
        GtkWidget *lapButton = timerData->lapButton;
        GtkWidget *pauseButton = timerData->pauseButton;

        gtk_button_set_label(GTK_BUTTON(pauseButton), "Pause");

        strftime(buffer, 256, "%I:%M:%S%P", timer->startLocalTime);
        windowTime = gtk_label_new(buffer);
        windowAction = gtk_label_new("Timer Started");
        windowElapsed = gtk_label_new("--");
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_hide(startButton);
        gtk_widget_show(pauseButton);
        gtk_widget_show(lapButton);
        gtk_widget_show(stopButton);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);
        displayWorkingRequest(buffer, grid, window, timerData);
    }
}

void stopTimerPressed (GtkWidget *widget, InterfacePointer timerData)
{
    TimerP timer = timerData->timerPointer;

    if(stopTimer(timer)) {
        getElapsedTime(timer);
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = timerData->grid;
        GtkWidget *window = timerData->window;
        GtkWidget *startButton = timerData->startButton;
        GtkWidget *stopButton = timerData->stopButton;
        GtkWidget *lapButton = timerData->lapButton;
        GtkWidget *pauseButton = timerData->pauseButton;

        strftime(buffer, 256, "%I:%M:%S%P", timer->stopLocalTime);
        windowTime = gtk_label_new(buffer);

        gchar actionString[256];
        sprintf(actionString, "Timer Stopped (%s)", timer->subject);
        windowAction = gtk_label_new(actionString);

        windowElapsed = gtk_label_new(timer->elapsedTime);
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_hide(stopButton);
        gtk_widget_hide(lapButton);
        gtk_widget_hide(pauseButton);
        gtk_widget_show(startButton);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);
    }
}

void updateStartTime(
    gchar time[256],
    GtkWidget *grid,
    GtkWidget *window,
    const gchar *text,
    InterfacePointer timerData
) {
    GtkWidget *windowTime;
    GtkWidget *windowAction;
    GtkWidget *windowElapsed;

    windowTime = gtk_label_new(time);
    windowAction = gtk_label_new("Timer Started");
    strlen(text);

    TimerP timer = timerData->timerPointer;
    sprintf(timer->subject, "%s", text);

    windowElapsed = gtk_label_new(text);
    gtk_grid_remove_row(GTK_GRID(grid), 0);
    gtk_grid_insert_row(GTK_GRID(grid), 0);
    gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);

    gtk_widget_show(windowTime);
    gtk_widget_show(windowAction);
    gtk_widget_show(windowElapsed);
}
