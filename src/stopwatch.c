#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>
#include <cairo.h>

#include "Timer.h"

int main (int argc, char *argv[]) {
    /**
     * Load up any parameters that may have been passed in from the
     * command line
     */
    gtk_init (&argc, &argv);
    initializeObjects();

    /**
     * Required for all GTK apps. Main loop that waits for an event to occur.
     * Continues until gtk_main_quit is called
     */
    gtk_main ();

    return 0;
}

