
#include <gtk/gtk.h>

static void hello( GtkWidget *widget,
                   gpointer   data )
{
	GtkWidget *dialog, *label;
	GtkDialogFlags flags;

	// Create the widgets
	flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	dialog = gtk_dialog_new_with_buttons ("Storage Info",
										  NULL,
										  flags,
										  ("_OK"),
										  GTK_RESPONSE_OK,
										  NULL);
	
	// Ensure that the dialog box is destroyed when the user responds
	g_signal_connect_swapped (dialog,
						  "response",
						  G_CALLBACK (gtk_widget_destroy),
						  dialog);

	// Set dialog size
	gtk_widget_set_size_request (dialog,
							      600,
								  400);

	// Add the label, and show everything we’ve added
	gtk_dialog_run (GTK_DIALOG (dialog));
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    g_print ("delete event occurred\n");

    return TRUE;
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    g_print ("destory \n");
    gtk_main_quit ();
}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *button;

    gtk_init (&argc, &argv);
    
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (delete_event), NULL);
    
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
    button = gtk_button_new_with_label ("Hello World");
    
    g_signal_connect (button, "clicked",
		      G_CALLBACK (hello), NULL);
    
//    g_signal_connect_swapped (button, "clicked",
//			      G_CALLBACK (gtk_widget_destroy),
//                              window);
    
    gtk_container_add (GTK_CONTAINER (window), button);
    
    gtk_widget_show (button);
    
    gtk_widget_show (window);
    
    gtk_main ();
    
    return 0;
}
