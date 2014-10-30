#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>
#include <cairo.h>

/* @TODO Global variables that should be moved */
gboolean running = FALSE;


static void startTimer (GtkWidget *widget, gpointer data) {
	g_print ("Start timer\n");
}

static void stopTimer (GtkWidget *widget, gpointer data) {
	g_print ("Stop timer\n");
}

gchar getTime() {
	time_t currentTime;
	struct tm *localTime;
	gchar buffer[256];

	currentTime = time(NULL);
	localTime = localtime(&currentTime);
	strftime(buffer, 256, "%I:%M:%S%P", localTime);
	return *buffer;
}

int main (int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *startButton;
	GtkWidget *stopButton;
	GtkWidget *windowTime;
	GtkWidget *windowAction;
	GtkWidget *windowElapsed;
	gchar buffer[256];
	time_t currentTime;
	struct tm *localTime;

	/* This is called in all GTK applications. Arguments are parsed
	* from the command line and are returned to the application.
	*/
	gtk_init (&argc, &argv);

	/* create a new window, and set its title */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
	gtk_window_set_title (GTK_WINDOW (window), "Stopwatch");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 20);

	/* Here we construct the container that is going pack our buttons */
	grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);

	/* Pack the container in the window */
	gtk_container_add (GTK_CONTAINER (window), grid);

	//Time and date stuff
	currentTime = time(NULL);
	localTime = localtime(&currentTime);
	strftime(buffer, 256, "%I:%M:%S%P", localTime);


	// Creating the text windows
	*buffer = getTime();
	windowTime = gtk_label_new(buffer);
	windowAction = gtk_label_new("Program Started");
	windowElapsed = gtk_label_new("--");

	gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);

	/* Place the first button in the grid cell (0, 0), and make it fill
	* just 1 cell horizontally and vertically (ie no spanning)
	*/
	//gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

	startButton = gtk_button_new_with_label ("Start");
	stopButton = gtk_button_new_with_label ("Stop");
	g_signal_connect (startButton, "clicked", G_CALLBACK (startTimer), NULL);
	g_signal_connect (stopButton, "clicked", G_CALLBACK (stopTimer), NULL);

	/* Place the second button in the grid cell (1, 0), and make it fill
	* just 1 cell horizontally and vertically (ie no spanning)
	*/
	gtk_grid_attach (GTK_GRID (grid), startButton, 0, 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), stopButton, 1, 1, 1, 1);

	/* Now that we are done packing our widgets, we show them all
	* in one go, by calling gtk_widget_show_all() on the window.
	* This call recursively calls gtk_widget_show() on all widgets
	* that are contained in the window, directly or indirectly.
	*/
	gtk_widget_show_all (window);

	/* All GTK applications must have a gtk_main(). Control ends here
	* and waits for an event to occur (like a key press or a mouse event),
	* until gtk_main_quit() is called.
	*/
	gtk_main ();

	return 0;
}
