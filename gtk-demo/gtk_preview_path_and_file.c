#include <gtk/gtk.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

//窗口退出信号的处理函数
void destroy_main_wnd(GtkWidget* widget, gpointer data);

//根据路径编辑框中的目录路径显示该目录下的文件
int init_file_list(GtkList* file_list, GtkEntry* directory_entry);

//当文件列表中的选择项发生改变时的处理函数
void select_file_changed(GtkList* file_list, gpointer data);

//双击文件列表中的项目时的处理函数
gint file_list_click_handle(GtkWidget* widget, GdkEventButton* event, gpointer data);

//返回按钮点击时的处理函数
void back_btn_clicked(GtkButton* back_btn, gpointer data);

GtkWidget* file_list;    //文件列表
GtkWidget* text;        //文本显示区
GtkWidget* directory_entry; //路径编辑器

//和List中的item关联数据时需要key
//第一个key和item的label关联
//第二个key和item的类型关联(属于目录还是普通文件)
const gchar *list_item_data_key_string="list_item_data_label_string";
const    gchar    *list_item_data_key_type="list_item_data_type";

int main(int argc, char* argv[])
{
    GtkWidget* window;                    //主窗口
    GtkWidget* list_scroll_wnd;            //用来放文件列表的滚动窗
    GtkWidget* text_view_scroll_wnd;    //用来放文本显示的滚动窗
    GtkWidget* vbox;                    
    GtkWidget* hbox;
    GtkWidget* valign;
    GtkWidget* halign;
    GtkWidget* directory_label;
    GtkWidget* btn_back;

    gtk_init(&argc, &argv);

    window = (GtkWidget*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
    list_scroll_wnd = (GtkWidget*)gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow*)(list_scroll_wnd), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    text_view_scroll_wnd = (GtkWidget*)gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow*)(text_view_scroll_wnd), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vbox = gtk_vbox_new(FALSE, 5);
    hbox = gtk_hbox_new(FALSE, 5);
    valign = gtk_alignment_new(0, 0, 1, 0);
    halign = gtk_alignment_new(1, 0, 0, 0);
    text = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)text, FALSE);
    file_list = gtk_list_new();
    gtk_list_set_selection_mode((GtkList*)file_list, GTK_SELECTION_BROWSE);
    directory_label = gtk_label_new("路径");
    directory_entry = gtk_entry_new();
    gtk_entry_set_editable((GtkEntry*)(directory_entry), FALSE);
    gtk_entry_set_text((GtkEntry*)(directory_entry), "/");
    btn_back = gtk_button_new_with_label("<<");

    if(0 != init_file_list((GtkList*)(file_list), (GtkEntry*)(directory_entry)))
    {
        g_print("init file list failed!\n");
        return 1;
    }

    gtk_window_set_position((GtkWindow*)window, GTK_WIN_POS_CENTER);
    gtk_window_set_default_size((GtkWindow*)window, 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    gtk_widget_set_size_request(directory_label, 40, 30);
    gtk_widget_set_size_request(btn_back, 40, 30);

    gtk_container_add(GTK_CONTAINER(halign), directory_label);
    gtk_box_pack_start(GTK_BOX(hbox), halign, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), directory_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), btn_back, FALSE, FALSE, 0);


    gtk_container_add(GTK_CONTAINER(valign), hbox);
    gtk_box_pack_start(GTK_BOX(vbox), valign, FALSE, FALSE, 0);
    gtk_scrolled_window_add_with_viewport((GtkScrolledWindow*)list_scroll_wnd, file_list);
    gtk_box_pack_start(GTK_BOX(vbox), list_scroll_wnd, TRUE, TRUE, 0);
    gtk_scrolled_window_add_with_viewport((GtkScrolledWindow*)text_view_scroll_wnd, text);
    gtk_box_pack_start(GTK_BOX(vbox), text_view_scroll_wnd, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy_main_wnd), NULL);
    g_signal_connect(G_OBJECT(file_list), "selection-changed", G_CALLBACK(select_file_changed), (gpointer)(text));
    g_signal_connect(G_OBJECT(file_list), "button_press_event", G_CALLBACK(file_list_click_handle), (gpointer)(directory_entry));
    g_signal_connect(G_OBJECT(btn_back), "clicked", G_CALLBACK(back_btn_clicked), (gpointer)(directory_entry));

    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

void destroy_main_wnd(GtkWidget* widget, gpointer data)
{
    gtk_main_quit();
}

GtkWidget* list_item_directory_new(const char* dirname)
{
    GtkWidget* list_item = gtk_list_item_new();
    GtkWidget* hbox = gtk_hbox_new(FALSE, 5);
    GtkWidget* image = gtk_image_new_from_stock(GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget* label = gtk_label_new(dirname);
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(list_item), hbox);
    gtk_object_set_data(GTK_OBJECT(list_item),
            list_item_data_key_string,
            (gpointer)dirname);

    gtk_object_set_data(GTK_OBJECT(list_item),
            list_item_data_key_type,
            (gpointer)0);
    return list_item;
}

GtkWidget* list_item_file_new(const char* filename)
{
    GtkWidget* list_item = gtk_list_item_new();
    GtkWidget* hbox = gtk_hbox_new(FALSE, 5);
    GtkWidget* image = gtk_image_new_from_stock(GTK_STOCK_FILE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget* label = gtk_label_new(filename);
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(list_item), hbox);
    gtk_object_set_data(GTK_OBJECT(list_item),
            list_item_data_key_string,
            (gpointer)filename);
    gtk_object_set_data(GTK_OBJECT(list_item), 
            list_item_data_key_type,
            (gpointer)1);
    return list_item;
}

int init_file_list(GtkList* file_list, GtkEntry* directory_entry)
{
    const char* dirpath = (const char*)gtk_entry_get_text(directory_entry);
    DIR* dir = opendir(dirpath);
    struct dirent* enump = NULL;
    GList* dlist = NULL;
    GList* flist = NULL;
    GtkWidget* list_item = NULL;
    size_t name_len;

    if(NULL == dir)
    {
        g_print("Open directory failed:%s.\n", dirpath);
        return 1;
    }

    while(enump = readdir(dir))
    {
        name_len = strlen(enump->d_name);

        if( (name_len == 1 && enump->d_name[0] == '.') 
                || (name_len == 2 && !strncmp(enump->d_name, "..", 2)))
            continue;

        if(DT_DIR == enump->d_type)
        {
            list_item = list_item_directory_new(enump->d_name);
            dlist = g_list_prepend(dlist, list_item);
        }

        if(DT_REG == enump->d_type)
        {
            list_item = list_item_file_new(enump->d_name);
            flist = g_list_prepend(flist, list_item);
        }
    }

    gtk_list_clear_items(file_list, 0, -1);
    gtk_list_append_items(file_list, dlist);
    gtk_list_append_items(file_list, flist);
    gtk_widget_show_all((GtkWidget*)(file_list));

    return 0;
}

void select_file_changed(GtkList* file_list, gpointer data)
{
    GtkTextView*     text = (GtkTextView*)data;
    GList*            dlist = GTK_LIST(file_list)->selection;
    GtkObject*        selected_item = NULL;
    gchar*            item_data_string;
    const gchar*     cur_dir;
    gchar*             path = NULL;
    struct stat     stat_buf;

    if(!dlist)
    {
        return;
    }

    selected_item = GTK_OBJECT(dlist->data);
    item_data_string = (gchar*)gtk_object_get_data(selected_item,
            list_item_data_key_string);
    gint type = (gint)gtk_object_get_data(selected_item, list_item_data_key_type);
    cur_dir = gtk_entry_get_text((GtkEntry*)directory_entry);

    path = (gchar*)malloc(strlen(cur_dir) + strlen(item_data_string) + 1);
    if(NULL == path)
        goto end_proc;
    strcpy(path, cur_dir);
    strcat(path, item_data_string);

    if(stat(path, &stat_buf) < 0)
        goto end_proc;

    if(0 == type)
    {
        //directory

    }
    else if(1==type)
    {
        //regular file

    }
end_proc:
    free(path);
}

//0 open failed
//1 open succeeded

int test_open(const char* path)
{
    struct dirent* enump = NULL;
    DIR* dir = opendir(path);
    if(NULL == dir)
    {
        return 0;
    }
    enump = readdir(dir);
    return NULL != enump;
}
void text_view_file_content(GtkWidget* text_view, const gchar* file_path)
{
    struct stat stat_buf;
    FILE* fp;
    size_t buf_size = 1024 * 4;
    GtkTextBuffer* text_buf = NULL;
    char* buf, *utf8_end;
    int i;

    if(stat(file_path, &stat_buf) < 0)
        return;
    
    if(NULL == (fp = fopen(file_path, "rb")))
        return;
    
    if(stat_buf.st_size < buf_size)
    {
        buf_size = stat_buf.st_size;
    }

    buf = (char*)malloc(buf_size);
    if(NULL == buf)
        return;
    
    if(fread(buf, 1, buf_size, fp) < buf_size)
        goto end_text_view_file_content;

    text_buf = gtk_text_view_get_buffer((GtkTextView*)text);
    if(NULL == text_buf)
    {
        text_buf = gtk_text_buffer_new(NULL);
        gtk_text_view_set_buffer((GtkTextView*)text, text_buf);
    }

    if(g_utf8_validate((const gchar*)buf, buf_size, (const gchar**)&utf8_end))
    {
        gtk_text_buffer_set_text(text_buf, (const gchar*)buf, buf_size);
    }
    else
    {
        for(i = 0; i < buf_size; ++i)
        {
            if(!isprint(buf[i]))
            {
                buf[i] = '.';
            }
        }
        gtk_text_buffer_set_text(text_buf, (const gchar*)buf, buf_size);
    }
    

end_text_view_file_content:
    free(buf);
}

gint file_list_click_handle(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
    if(GTK_IS_LIST(widget) && event->type == GDK_2BUTTON_PRESS)
    {
        GList* dlist = GTK_LIST(widget)->selection;
        GtkEntry* path_entry = (GtkEntry*)data;

        if(NULL != dlist)
        {
            GtkObject* selected_item = GTK_OBJECT(dlist->data);
            const char* old_path = gtk_entry_get_text(path_entry);
            const char* selected_item_string = (gchar*)gtk_object_get_data(selected_item, list_item_data_key_string);
            gint type = (gint)gtk_object_get_data(selected_item, list_item_data_key_type);

            if(0 == type)
            {
                //directory
                gchar* new_path = (gchar*)malloc(strlen(old_path) + strlen(selected_item_string) + 2);
                strcpy(new_path, old_path);
                strcat(new_path, selected_item_string);
                strcat(new_path, "/");

                if(test_open(new_path))
                {
                    gtk_entry_set_text(path_entry, new_path);
                    init_file_list((GtkList*)widget, path_entry);
                }

                free(new_path);
            }
            else if(1 == type)
            {
                //regular file
                gchar* file_path = (gchar*)malloc(strlen(old_path) + strlen(selected_item_string) + 1);
                strcpy(file_path, old_path);
                strcat(file_path, selected_item_string);
                text_view_file_content(text, file_path);
            }
        }
    }
}

//calc the position of the second last slash
// calculated start pos 
// |
// /usr/bin/

gint calc_start_pos(const char* path)
{
    size_t path_len = strlen(path);
    const char* p = path;
    gint count = 0;
    const char* p_second_last_slash = path + path_len - 2;
    while(*p_second_last_slash != '/')
    {
        p_second_last_slash--;
    }

    for(; p != p_second_last_slash; ++p)
    {
        count++;
    }

    //skip the slash
    count+=1;

    return count;
}

void back_btn_clicked(GtkButton* back_btn, gpointer data)
{
    GtkEntry* path_entry = (GtkEntry*)data;
    gint start_pos;
    const char* old_path = gtk_entry_get_text(path_entry);
    if(!strcmp(old_path, "/"))
        return;

    start_pos = calc_start_pos(old_path);

    gtk_editable_delete_text((GtkEditable*)path_entry, start_pos, -1);

    init_file_list((GtkList*)(file_list), (GtkEntry*)(path_entry));
}
