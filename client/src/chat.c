#include <gtk/gtk.h>

// Открытие окна чата
void create_chat_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Чат");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Поле для отображения сообщений
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    // Поле для ввода сообщения
    GtkWidget *entry_message = gtk_entry_new();

    // Кнопка "Отправить"
    GtkWidget *button_send = gtk_button_new_with_label("Отправить");

    // Добавляем виджеты в сетку
    gtk_grid_attach(GTK_GRID(grid), text_view, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_message, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button_send, 1, 1, 1, 1);

    gtk_widget_show_all(window);
}

