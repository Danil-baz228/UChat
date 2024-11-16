#include <gtk/gtk.h>
#include "../inc/client.h"

// Переменная для хранения буфера текстового виджета
GtkTextBuffer *chat_buffer = NULL;


void on_logout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);

    // Destroy the current chat window
    gtk_widget_destroy(window);
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

    // Метка для имени пользователя
    char user_label_text[128];
    snprintf(user_label_text, sizeof(user_label_text), "Пользователь: %s", current_user);
    GtkWidget *user_label = gtk_label_new(user_label_text);
    gtk_box_pack_start(GTK_BOX(header_box), user_label, TRUE, TRUE, 0);

    // Кнопка выхода
    GtkWidget *logout_button = gtk_button_new_with_label("Выйти");
    gtk_box_pack_end(GTK_BOX(header_box), logout_button, FALSE, FALSE, 0);

    // Обработчик кнопки выхода
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

    // Обработчик выбора пользователя
    g_signal_connect(users_list, "row-selected", G_CALLBACK(on_user_selected), window);

    // Правая часть с чатом
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), right_box, TRUE, TRUE, 0);

    // Поле для отображения сообщений
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

    // Поле ввода сообщения
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(right_box), message_box, FALSE, FALSE, 0);

    GtkWidget *entry_message = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Введите сообщение");
    gtk_widget_set_hexpand(entry_message, TRUE);
    gtk_box_pack_start(GTK_BOX(message_box), entry_message, TRUE, TRUE, 0);

    GtkWidget *button_send = gtk_button_new_with_label("Отправить");
    gtk_box_pack_start(GTK_BOX(message_box), button_send, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    // Загрузка списка пользователей
    load_users_list(users_list);

    // Таймер для обновления сообщений каждые 2 секунды
    g_timeout_add(2000, (GSourceFunc)update_chat_window, window);

    // Привязываем данные к виджетам
    g_object_set_data(G_OBJECT(window), "text_view", text_view);
    g_object_set_data(G_OBJECT(window), "entry_message", entry_message);
    g_object_set_data(G_OBJECT(window), "users_list", users_list);
    g_object_set_data(G_OBJECT(window), "current_user", current_user);
    g_object_set_data(G_OBJECT(window), "selected_user", NULL);

    // Обработчик для отправки сообщений
    g_signal_connect(button_send, "clicked", G_CALLBACK(on_send_message_clicked), window);
}
