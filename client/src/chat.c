/// chat.c
#include <gtk/gtk.h>
#include "../inc/client.h"

// Переменная для хранения буфера текстового виджета
GtkTextBuffer *chat_buffer = NULL;

void on_logout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);
    gtk_widget_destroy(window); // Закрываем окно чата
}

void create_chat_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Чат");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Основной вертикальный контейнер
    GtkWidget *main_vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(main_vertical_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_vertical_box);

    // Верхняя панель с именем пользователя
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(header_box), TRUE);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), header_box, FALSE, FALSE, 0);

    char user_label_text[128];
    snprintf(user_label_text, sizeof(user_label_text), "Пользователь: %s", current_user);
    GtkWidget *user_label = gtk_label_new(user_label_text);
    gtk_box_pack_start(GTK_BOX(header_box), user_label, TRUE, TRUE, 0);

    GtkWidget *logout_button = gtk_button_new_with_label("Выйти");
    gtk_box_pack_end(GTK_BOX(header_box), logout_button, FALSE, FALSE, 0);
    g_signal_connect(logout_button, "clicked", G_CALLBACK(on_logout_clicked), window);

    // Основной горизонтальный контейнер
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), main_box, TRUE, TRUE, 0);

    // Список пользователей (левая панель)
    GtkWidget *users_list = gtk_list_box_new();
    gtk_widget_set_vexpand(users_list, TRUE);

    GtkWidget *scrolled_users = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_users), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_users, 200, -1);
    gtk_container_add(GTK_CONTAINER(scrolled_users), users_list);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled_users, FALSE, TRUE, 0);

    g_signal_connect(users_list, "row-selected", G_CALLBACK(on_user_selected), window);

    // Правая часть с чатом
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), right_box, TRUE, TRUE, 0);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    chat_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(chat_buffer, "Добро пожаловать в чат!\n", -1);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(right_box), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(right_box), message_box, FALSE, FALSE, 0);

    GtkWidget *entry_message = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Введите сообщение");
    gtk_widget_set_hexpand(entry_message, TRUE);
    gtk_box_pack_start(GTK_BOX(message_box), entry_message, TRUE, TRUE, 0);

    GtkWidget *button_send = gtk_button_new_with_label("Отправить");
    gtk_box_pack_start(GTK_BOX(message_box), button_send, FALSE, FALSE, 0);

    // Кнопка для выбора стикеров
    GtkWidget *sticker_button = gtk_button_new_with_label("🙂");
    gtk_box_pack_start(GTK_BOX(message_box), sticker_button, FALSE, FALSE, 0);
    GtkWidget *sticker_window = create_sticker_window(window, entry_message); // Создаем окно для стикеров

    g_signal_connect(sticker_button, "clicked", G_CALLBACK(update_button_position), sticker_window);
	g_signal_connect(sticker_button, "clicked", G_CALLBACK(show_sticker_window), sticker_window);

	// Добавление CSS стилей
    const char *css_style =
        "window { background-color: #ECEFF4; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }"
                "label { font-size: 16px; color: #2E3440; font-weight: bold; }"
                "entry { border: 2px solid #4C566A; border-radius: 5px; padding: 10px; margin-bottom: 15px; background-color: #D8DEE9; color: #2E3440; font-size: 14px; }"
                "entry:focus { border-color: #3B4252; }"
                "button { background-color: #5E81AC; color: white; border-radius: 5px; padding: 10px 20px; font-weight: bold; transition: background-color 0.3s ease; font-size: 14px; }"
                "button:hover { background-color: #81A1C1; }"
                "scrolledwindow { background-color: #D8DEE9; border-radius: 5px; }"
                "listbox { background-color: #D8DEE9; color: #2E3440; border-radius: 5px; padding: 10px; margin-top: 10px; }"
                "listbox row:hover { background-color: #B2B9C0; }";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_style, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(window);

    load_users_list(users_list);

    g_timeout_add(2000, (GSourceFunc)update_chat_window, window);

    g_object_set_data(G_OBJECT(window), "text_view", text_view);
    g_object_set_data(G_OBJECT(window), "entry_message", entry_message);
    g_object_set_data(G_OBJECT(window), "users_list", users_list);
    g_object_set_data(G_OBJECT(window), "current_user", current_user);
    g_object_set_data(G_OBJECT(window), "selected_user", NULL);

    g_signal_connect(button_send, "clicked", G_CALLBACK(on_send_message_clicked), window);
}


void update_button_position(GtkButton *button, GtkWidget *sticker_window) {
    while (gtk_events_pending()) {
        gtk_main_iteration_do(FALSE); // Обновляем интерфейс
    }

    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(toplevel)) {
        g_print("Ошибка: Toplevel не является окном\n");
        return;
    }

    // Считываем координаты кнопки
    int button_x, button_y;
    gtk_widget_translate_coordinates(GTK_WIDGET(button), toplevel, 0, 0, &button_x, &button_y);

    g_object_set_data(G_OBJECT(sticker_window), "button_x", GINT_TO_POINTER(button_x));
    g_object_set_data(G_OBJECT(sticker_window), "button_y", GINT_TO_POINTER(button_y));
    g_print("Обновлены координаты кнопки: (%d, %d)\n", button_x, button_y);
}


