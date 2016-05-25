#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>
#include <cairo.h>
#include <stdlib.h>

typedef struct Timer {
	gboolean running;
}*TimerP;

typedef struct TimerData {
    TimerP timerPointer;
} *TimerDataP;

static void startTimerPressed (GtkWidget *widget, gpointer data, TimerDataP timerData) {
    printf("TimerData Location: %p\n", timerData);
    TimerP timer = timerData->timerPointer;
}

int main (int argc, char *argv[]) {
    GtkWidget *startButton;
    GtkWidget *window;
    GtkWidget *grid;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 550, 200);
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 20);

	grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_container_add (GTK_CONTAINER (window), grid);

	startButton = gtk_button_new_with_label ("Start");

    TimerP timerPointer;
    timerPointer = malloc(sizeof(struct Timer));

    TimerDataP timerDataP;
    timerDataP = malloc(sizeof(struct TimerData));

    printf("TimerData Location: %p\n", timerDataP);

    g_signal_connect (startButton, "clicked", G_CALLBACK (startTimerPressed), timerDataP);
	gtk_grid_attach (GTK_GRID (grid), startButton, 0, 4, 1, 1);

	gtk_widget_show_all(window);

	gtk_main ();
}
