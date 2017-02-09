#include "Interface.h"

// Private function prototypes

/**
 * Returns a newly created grid that we will use for display purposes within
 * the window
 */
GtkWidget* createGrid();
/**
 * Create a GTK window that is the main display
 */
GtkWidget* createWindow();
/**
 * Displays the window for allowing the user to enter what they are working
 * on
 */
void displayWorkingRequest(gchar[256], InterfacePointer);
/**
 * Called when the lap button is pressed
 */
void lapButtonPressed (GtkWidget*, InterfacePointer);
/**
 * Called when the pause button is pressed
 */
void pauseButtonPressed (GtkWidget*, InterfacePointer);
/**
 * Called when the start button is pressed
 */
void startTimerPressed(GtkWidget*, InterfacePointer);
/**
 * Called when the stop button is pressed
 */
void stopTimerPressed(GtkWidget*, InterfacePointer);
/**
 * Called once the user has entered what they are working on so we can update
 * that row in the display
 */
void updateStartTime(const gchar*, InterfacePointer);

GtkWidget* createGrid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    return grid;
}

GtkWidget* createWindow() {
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(
        GTK_WINDOW(window),
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );
    gtk_window_set_title (GTK_WINDOW (window), "Stopwatch");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);
    return window;
}

void displayWorkingRequest (
    gchar currentText[256],
    InterfacePointer interface
) {
    GtkWidget *dialog;
    GtkWidget *textEntry;
    GtkWidget *label;

    label = gtk_label_new("What Are You Working On?");

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons(
        "Starting Timer",
        GTK_WINDOW(interface->window),
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
        gtk_entry_get_text(GTK_ENTRY(textEntry)),
        interface
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

    InterfacePointer interface;
    interface = malloc(sizeof(struct Interface));
    interface->timerPointer = timerPointer;
    interface->grid = grid;
    interface->window = window;
    interface->startButton = startButton;
    interface->stopButton = stopButton;
    interface->lapButton = lapButton;
    interface->pauseButton = pauseButton;

    g_signal_connect (
        startButton,
        "clicked",
        G_CALLBACK (startTimerPressed),
        interface
    );
    g_signal_connect (
        stopButton,
        "clicked",
        G_CALLBACK (stopTimerPressed),
        interface
    );
    g_signal_connect (
        lapButton,
        "clicked",
        G_CALLBACK(lapButtonPressed),
        interface
    );
    g_signal_connect (
        pauseButton,
        "clicked",
        G_CALLBACK(pauseButtonPressed),
        interface
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

/**
 * @TODO A bunch of this should be moved into the timer object
 */
void lapButtonPressed (GtkWidget *widget, InterfacePointer interface)
{
    gchar buffer[256];
    gchar currentElapsed[256];
    GtkWidget *windowTime, *windowAction, *windowElapsed;
    TimerP timer = interface->timerPointer;

    loadCurrentElapsedTime(currentElapsed, timer);

    loadCurrentLocalTime(buffer, 256);
    windowTime = gtk_label_new(buffer);

    sprintf(buffer, "Displaying Time (%s)", timer->subject);
    windowAction = gtk_label_new(buffer);

    windowElapsed = gtk_label_new(currentElapsed);
    gtk_grid_remove_row(GTK_GRID(interface->grid), 3);
    gtk_grid_insert_row(GTK_GRID(interface->grid), 0);
    gtk_grid_attach(GTK_GRID(interface->grid), windowTime, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(interface->grid), windowAction, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(interface->grid), windowElapsed, 2, 0, 1, 1);
    gtk_widget_show(windowTime);
    gtk_widget_show(windowAction);
    gtk_widget_show(windowElapsed);
}

void pauseButtonPressed (GtkWidget *widget, InterfacePointer interface)
{
    TimerP timer = interface->timerPointer;

    // Timer isn't running, pause button shouldn't be visible
    if (isStopped(timer)) {
        gtk_widget_hide(interface->pauseButton);
    }

    if (!interface->timerPointer->paused) {
        pauseTimer(interface->timerPointer);

        gtk_widget_hide(interface->lapButton);

        gchar buffer[256];
        loadStopLocalTime(timer, buffer, 256);
        GtkWidget *windowTime;
        windowTime = gtk_label_new(buffer);

        gchar actionString[256];
        sprintf(actionString, "Timer paused (%s)", timer->subject);
        GtkWidget *windowAction;
        windowAction = gtk_label_new(actionString);

        GtkWidget *windowElapsed;
        windowElapsed = gtk_label_new(timer->elapsedTime);
        GtkWidget *grid = interface->grid;
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);

        gtk_button_set_label(GTK_BUTTON(interface->pauseButton), "Resume");
    } else {
        resumeTimer(interface->timerPointer);
        gtk_widget_show(interface->lapButton);

        TimerP timer = interface->timerPointer;
        gchar buffer[256];
        loadStartLocalTime(timer, buffer, 256);
        GtkWidget *windowTime;
        windowTime = gtk_label_new(buffer);

        gchar actionString[256];
        sprintf(actionString, "Timer resumed (%s)", timer->subject);
        GtkWidget *windowAction;
        windowAction = gtk_label_new(actionString);

        GtkWidget *windowElapsed;
        windowElapsed = gtk_label_new(timer->elapsedTime);
        GtkWidget *grid = interface->grid;
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);


        gtk_button_set_label(GTK_BUTTON(interface->pauseButton), "Pause");
    }
}

void startTimerPressed (GtkWidget *widget, InterfacePointer interface)
{
    TimerP timer = interface->timerPointer;

    gboolean success = startTimer(timer);
    if(success) {
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = interface->grid;
        GtkWidget *window = interface->window;
        GtkWidget *startButton = interface->startButton;
        GtkWidget *stopButton = interface->stopButton;
        GtkWidget *lapButton = interface->lapButton;
        GtkWidget *pauseButton = interface->pauseButton;

        gtk_button_set_label(GTK_BUTTON(pauseButton), "Pause");

        loadStartLocalTime(timer, buffer, 256);
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
        displayWorkingRequest(buffer, interface);
    }
}

void stopTimerPressed (GtkWidget *widget, InterfacePointer interface)
{
    TimerP timer = interface->timerPointer;

    if(stopTimer(timer)) {
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = interface->grid;
        GtkWidget *window = interface->window;
        GtkWidget *startButton = interface->startButton;
        GtkWidget *stopButton = interface->stopButton;
        GtkWidget *lapButton = interface->lapButton;
        GtkWidget *pauseButton = interface->pauseButton;

        loadStopLocalTime(timer, buffer, 256);
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
    const gchar *text,
    InterfacePointer interface
) {
    GtkWidget *grid, *windowElapsed;

    grid = interface->grid;

    TimerP timer = interface->timerPointer;
    sprintf(timer->subject, "%s", text);

    windowElapsed = gtk_grid_get_child_at(GTK_GRID(grid), 2, 0);
    gtk_label_set_text(GTK_LABEL(windowElapsed), text);
}
