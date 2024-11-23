#include <gtk/gtk.h>
#include "../inc/client.h"

GtkWidget *create_sticker_window(GtkWidget *parent_window, GtkWidget *entry_message) {
    GtkWidget *sticker_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(sticker_window), "Стикеры");
    gtk_window_set_default_size(GTK_WINDOW(sticker_window), 150, 200);
    gtk_window_set_transient_for(GTK_WINDOW(sticker_window), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(sticker_window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(sticker_window), FALSE);

    g_signal_connect(sticker_window, "delete-event", G_CALLBACK(hide_sticker_window), NULL);

    // Прокручиваемый контейнер для стикеров
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_container_add(GTK_CONTAINER(sticker_window), scrolled_window);

    // Сетка для стикеров
    GtkWidget *sticker_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(sticker_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(sticker_grid), 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), sticker_grid);

    // Добавляем стикеры
    const char *stickers[] = {
        "😀", "😁", "😂", "🤣", "😃", "😄", "😅", "😆", "😉", "😊", "😋", "😎", "😍", "😘", "🥰", "😗",
        "😙", "😚", "🙂", "🤗", "🤩", "🤔", "🤨", "😐", "😑", "😶", "🙄", "😏", "😣", "😥", "😮", "🤐",
        "😯", "😪", "😫", "🥱", "😴", "😌", "😛", "😜", "😝", "🤤", "😒", "😓", "😔", "😕", "🙃", "🤑"
    };

    int num_stickers = sizeof(stickers) / sizeof(stickers[0]);

    for (int i = 0; i < num_stickers; i++) {
        GtkWidget *sticker_button = gtk_button_new_with_label(stickers[i]);
        g_signal_connect(sticker_button, "clicked", G_CALLBACK(on_sticker_selected), entry_message);
        gtk_grid_attach(GTK_GRID(sticker_grid), sticker_button, i % 3, i / 3, 1, 1);
    }

    return sticker_window;
}

void hide_sticker_window(GtkWidget *sticker_window, gpointer user_data) {
    gtk_widget_hide(sticker_window);
}
void show_sticker_window(GtkButton *button, gpointer user_data) {
    GtkWidget *sticker_window = GTK_WIDGET(user_data);

    if (gtk_widget_get_visible(sticker_window)) {
        gtk_widget_hide(sticker_window); // Скрыть, если окно уже открыто
        return;
    }

    // Обновляем события перед вычислением координат
    while (gtk_events_pending()) {
        gtk_main_iteration_do(FALSE);
    }

    // Получаем верхний уровень окна
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GdkWindow *gdk_toplevel = gtk_widget_get_window(toplevel);
    if (!GTK_IS_WINDOW(toplevel) || gdk_toplevel == NULL) {
        g_print("Ошибка: Toplevel или GdkWindow отсутствуют\n");
        return;
    }

    // Получаем глобальные координаты родительского окна
    int toplevel_x, toplevel_y;
    gdk_window_get_origin(gdk_toplevel, &toplevel_x, &toplevel_y);

    // Получаем координаты кнопки внутри родительского окна
    int button_x, button_y;
    gtk_widget_translate_coordinates(GTK_WIDGET(button), toplevel, 0, 0, &button_x, &button_y);

    // Глобальные координаты кнопки
    int global_button_x = toplevel_x + button_x;
    int global_button_y = toplevel_y + button_y;

    // Получаем размеры кнопки
    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(button), &allocation);

    // Устанавливаем позицию окна стикеров рядом с кнопкой
    int sticker_window_x = global_button_x;
    int sticker_window_y = global_button_y + allocation.height + 5; // Смещение вниз на 5 пикселей

    // Перемещаем и показываем окно стикеров
    gtk_window_move(GTK_WINDOW(sticker_window), sticker_window_x, sticker_window_y);
    gtk_widget_show_all(sticker_window);

    // Отладочный вывод для проверки
    g_print("Toplevel position: (%d, %d)\n", toplevel_x, toplevel_y);
    g_print("Button position: (%d, %d)\n", global_button_x, global_button_y);
    g_print("Sticker window position: (%d, %d)\n", sticker_window_x, sticker_window_y);
}

void on_sticker_selected(GtkButton *button, gpointer user_data) {
    const char *sticker = gtk_button_get_label(button);
    GtkEntry *entry_message = GTK_ENTRY(user_data);
    const char *current_text = gtk_entry_get_text(entry_message);

    char new_text[256];
    snprintf(new_text, sizeof(new_text), "%s%s", current_text, sticker);

    gtk_entry_set_text(entry_message, new_text);

    // Закрываем окно стикеров
    gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

