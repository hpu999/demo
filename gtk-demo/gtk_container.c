#include<gtk/gtk.h>
int main(int argc, char *argv[])
{
        GtkWidget *window;
        GtkWidget       *button1,*button2,*button3;
        GtkWidget       *box;
        gchar *str = "盒状容器";
        gtk_init (&argc, &argv);
 
        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title (GTK_WINDOW (window), "盒状容器");
        gtk_window_set_default_size(GTK_WINDOW(window),400,300);
        gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
        gtk_container_set_border_width(GTK_CONTAINER(window),40);
        g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
 
        box = gtk_hbox_new(FALSE,0);
        gtk_container_add(GTK_CONTAINER(window),box);
 
        button1 = gtk_button_new_with_label("我是按钮1");
        // gtk_box_pack_start(GTK_BOX(box),button1,TRUE,TRUE,0);
        gtk_container_add(GTK_CONTAINER(box),button1);
 
        button2 = gtk_button_new_with_label("我是按钮2");
        // gtk_box_pack_start(GTK_BOX(box),button2,TRUE,TRUE,0);
        gtk_container_add(GTK_CONTAINER(box),button2);
 
        button3 = gtk_button_new_with_label("我是按钮3");
        // gtk_box_pack_start(GTK_BOX(box),button3,TRUE,TRUE,0);
        gtk_container_add(GTK_CONTAINER(box),button3);
 
        gtk_widget_show_all(window);
        gtk_main ();
        return 0;
}

