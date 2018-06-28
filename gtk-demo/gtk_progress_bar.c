#include <gtk/gtk.h>
#include <string.h>
 
// 回调函数，切换进度条的移动方向 
void toggle_orientation(GtkWidget *widget, gpointer data) 
{ 
	// gtk_progress_bar_get_orientation: 获得进度条当前移动的方向
	switch( gtk_progress_bar_get_orientation( GTK_PROGRESS_BAR(data) ) ){ 
	case GTK_PROGRESS_LEFT_TO_RIGHT: 
		gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data), GTK_PROGRESS_RIGHT_TO_LEFT); 
		break; 
	case GTK_PROGRESS_RIGHT_TO_LEFT: 
		gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data), GTK_PROGRESS_LEFT_TO_RIGHT); 
		break; 
	default: // 什么也不做 
		break;
	} 
} 
 
// 更新进度条，这样就能够看到进度条的移动
void callback(GtkWidget *widget, gpointer data)
{
	// 在原来值基础上增加 0.05
	gdouble new_val = gtk_progress_bar_get_fraction( GTK_PROGRESS_BAR(data) ) + 0.05; 
	   
	if(new_val > 1.0){	// 越界处理
		new_val = 0.0; 
	}
	   
	// 设置进度条的新值 
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data), new_val); 
}
 
int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);		// 初始化
 
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);		// 创建主窗口
	gtk_window_set_title(GTK_WINDOW(window), "GtkProgressBar");		// 设置窗口标题
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);		// 设置边框宽度
 
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window, 300, 200);
	// 窗口关联 destroy 信号 到 gtk_main_quit
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
 
	GtkWidget *vbox = gtk_vbox_new(FALSE, 5);		 // 垂直布局容器
	gtk_container_add(GTK_CONTAINER(window), vbox);  // 容器加入窗口
 
	// 创建一个进度条
	GtkWidget *progress = gtk_progress_bar_new();
	gtk_container_add(GTK_CONTAINER(vbox), progress);  // 加入垂直布局容器
	// 设置进度条显示的百分比：50%
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.5 ); 
	// 设置在进度条的滑槽上的文本显示 
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "some text");
 
	// 添加一个按钮，切换移动方向  
	GtkWidget *button_orientation = gtk_button_new_with_label("Right to Left"); 
	g_signal_connect(button_orientation, "clicked", G_CALLBACK(toggle_orientation), progress); 
	gtk_container_add(GTK_CONTAINER(vbox), button_orientation);  // 加入垂直布局容器
 
	// 增加进度条进度按钮
	GtkWidget *button = gtk_button_new_with_label("add");
	g_signal_connect(button, "clicked", G_CALLBACK(callback), progress); // connect
	gtk_container_add(GTK_CONTAINER(vbox), button);  // 加入垂直布局容器
 
	gtk_widget_show_all(window);
 
	gtk_main();			// 主事件循环
 
	return 0;
}

