#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>

static void create_window (GtkWidget *widget, gpointer data) {
	GtkWidget *newWindow;

	newWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (newWindow), "New Window");
	gtk_widget_show_all (newWindow);
}

static void button_two (GtkWidget *widget, gpointer data) {
	g_print ("Button two\n");
}

int main (int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *button;
	GtkWidget *windowTime;
	GtkWidget *windowAction;
	GtkWidget *windowElapsed;
	GDate *date;
	GTimeVal time;
	gchar tmp_buffer[256];

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
	date = g_date_new();
	g_get_current_time(&time);
	g_date_set_time_val(date, &time);
	g_date_strftime(tmp_buffer, 256, "%r", date);
	g_print("Current date (heap):  %s\n", tmp_buffer);

	// Creating the text windows
	windowTime = gtk_label_new("04:08:13PM");
	windowAction = gtk_label_new("Started");
	windowElapsed = gtk_label_new("04 hours 56 minutes");

	gtk_grid_attach(GTK_GRID(grid), windowTime, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), windowAction, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), windowElapsed, 2, 0, 1, 1);

	button = gtk_button_new_with_label ("Create Window");
	g_signal_connect (button, "clicked", G_CALLBACK (create_window), NULL);

	/* Place the first button in the grid cell (0, 0), and make it fill
	* just 1 cell horizontally and vertically (ie no spanning)
	*/
	//gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

	button = gtk_button_new_with_label ("Button 2");
	g_signal_connect (button, "clicked", G_CALLBACK (button_two), NULL);

	/* Place the second button in the grid cell (1, 0), and make it fill
	* just 1 cell horizontally and vertically (ie no spanning)
	*/
	gtk_grid_attach (GTK_GRID (grid), button, 1, 1, 1, 1);

	button = gtk_button_new_with_label ("Quit");
	g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

	/* Place the Quit button in the grid cell (0, 1), and make it
	* span 2 columns.
	*/
	gtk_grid_attach (GTK_GRID (grid), button, 1, 2, 1, 1);

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
