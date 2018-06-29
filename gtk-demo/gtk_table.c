#include <gtk/gtk.h>
 
void close_app(GtkWidget *window,gpointer data)
{
    gtk_main_quit();
}
 
int main(int argc,char *argv[])
{
    GtkWidget *window;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *table;
 
 
    gtk_init(&argc,&argv);
//新建一个窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"Table demo");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
//    gtk_window_set_default_size(GTK_WINDOW(window),200,200);
    g_signal_connect(GTK_OBJECT(window),"destroy",G_CALLBACK(close_app),NULL);
 
//新建两个按钮和一个表
    button1 = gtk_button_new_with_label("button1");
    button2 = gtk_button_new_with_label("button2");
//表的大小是2*2。
	table = gtk_table_new(2,2,TRUE); 
 
//将两个按钮添加到表，button1占第一行的两个单元，button2占第二行的第一个单元
	gtk_table_attach(GTK_TABLE(table),button1,0,2,0,1,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),button2,0,1,1,2,GTK_FILL,GTK_FILL,0,0);
    gtk_container_add(GTK_CONTAINER(window),table);
 
    gtk_widget_show_all(window);
    gtk_main();
 
    return 0;
}

