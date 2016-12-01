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
 *
 * @TODO This can hopefully be changed to just update the row so we don't have
 * to have the time passed into it
 */
void updateStartTime(
    char[256], const gchar*, InterfacePointer
);

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
    InterfacePointer interfacePointer
) {
    GtkWidget *dialog;
    GtkWidget *textEntry;
    GtkWidget *label;

    label = gtk_label_new("What Are You Working On?");

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons(
        "Starting Timer",
        GTK_WINDOW(interfacePointer->window),
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
        currentText,
        gtk_entry_get_text(GTK_ENTRY(textEntry)),
        interfacePointer
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

void lapButtonPressed (GtkWidget *widget, InterfacePointer interfacePointer)
{
    gchar eventTimeString[256];
    gchar *currentElapsed;
    time_t currentTime;

    TimerP timer = interfacePointer->timerPointer;
    currentElapsed = getCurrentTime(timer);

    GtkWidget *windowTime;
    GtkWidget *windowAction;
    GtkWidget *windowElapsed;
    GtkWidget *grid = interfacePointer->grid;
    GtkWidget *window = interfacePointer->window;

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

void pauseButtonPressed (GtkWidget *widget, InterfacePointer interfacePointer)
{
    // Timer isn't running, pause button shouldn't be visible
    if (!interfacePointer->timerPointer->running) {
        gtk_widget_hide(interfacePointer->pauseButton);
    }

    if (!interfacePointer->timerPointer->paused) {
        pauseTimer(interfacePointer->timerPointer);

        gtk_widget_hide(interfacePointer->lapButton);

        TimerP timer = interfacePointer->timerPointer;
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
        GtkWidget *grid = interfacePointer->grid;
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);

        gtk_button_set_label(GTK_BUTTON(interfacePointer->pauseButton), "Resume");
    } else {
        resumeTimer(interfacePointer->timerPointer);
        gtk_widget_show(interfacePointer->lapButton);

        TimerP timer = interfacePointer->timerPointer;
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
        GtkWidget *grid = interfacePointer->grid;
        gtk_grid_remove_row(GTK_GRID(grid), 3);
        gtk_grid_insert_row(GTK_GRID(grid), 0);
        gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);
        gtk_widget_show(windowTime);
        gtk_widget_show(windowAction);
        gtk_widget_show(windowElapsed);


        gtk_button_set_label(GTK_BUTTON(interfacePointer->pauseButton), "Pause");
    }
}

void startTimerPressed (GtkWidget *widget, InterfacePointer interfacePointer)
{
    TimerP timer = interfacePointer->timerPointer;

    gboolean success = startTimer(timer);
    if(success) {
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = interfacePointer->grid;
        GtkWidget *window = interfacePointer->window;
        GtkWidget *startButton = interfacePointer->startButton;
        GtkWidget *stopButton = interfacePointer->stopButton;
        GtkWidget *lapButton = interfacePointer->lapButton;
        GtkWidget *pauseButton = interfacePointer->pauseButton;

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
        displayWorkingRequest(buffer, interfacePointer);
    }
}

void stopTimerPressed (GtkWidget *widget, InterfacePointer interfacePointer)
{
    TimerP timer = interfacePointer->timerPointer;

    if(stopTimer(timer)) {
        getElapsedTime(timer);
        gchar buffer[256];
        GtkWidget *windowTime;
        GtkWidget *windowAction;
        GtkWidget *windowElapsed;
        GtkWidget *grid = interfacePointer->grid;
        GtkWidget *window = interfacePointer->window;
        GtkWidget *startButton = interfacePointer->startButton;
        GtkWidget *stopButton = interfacePointer->stopButton;
        GtkWidget *lapButton = interfacePointer->lapButton;
        GtkWidget *pauseButton = interfacePointer->pauseButton;

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
    const gchar *text,
    InterfacePointer interfacePointer
) {
    GtkWidget *grid, *window, *windowAction, *windowTime, *windowElapsed;

    grid = interfacePointer->grid;
    window = interfacePointer->window;

    windowTime = gtk_label_new(time);
    windowAction = gtk_label_new("Timer Started");
    strlen(text);

    TimerP timer = interfacePointer->timerPointer;
    sprintf(timer->subject, "%s", text);

    windowElapsed = gtk_label_new(text);
    /**
     * @TODO Is it possible to update the row rather than removing and
     * inserting it?
     */
    gtk_grid_remove_row(GTK_GRID(grid), 0);
    gtk_grid_insert_row(GTK_GRID(grid), 0);
    gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);

    gtk_widget_show(windowTime);
    gtk_widget_show(windowAction);
    gtk_widget_show(windowElapsed);
}
