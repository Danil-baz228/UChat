// handlers.c
#include "../inc/client.h"
#include <gtk/gtk.h>
#include <string.h>

void scroll_to_bottom(GtkTextView *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextMark *mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_view_scroll_to_mark(text_view, mark, 0.0, TRUE, 0.0, 1.0);
}

void on_register_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entries[1]));


    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Ім'я користувача та пароль не повинні бути порожніми!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    char response[256];
    if (send_to_server("REGISTER", username, password, "", response, sizeof(response)) == 0) {
        if (strcmp(response, "OK") == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                       "Реєстрація пройшла успішно!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else if (strcmp(response, "USER_EXISTS") == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                       "Користувач із таким ім'ям вже існує!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                       "Помилка реєстрації!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Помилка підключення до сервера!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void load_users_list(GtkWidget *users_list) {
    // Очистка списка пользователей
    GList *children = gtk_container_get_children(GTK_CONTAINER(users_list));
    for (GList *child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);

    // Получение списка пользователей с сервера
    char response[1024];
    if (send_to_server("GET_USERS", "", "", "", response, sizeof(response)) != 0) {
        fprintf(stderr, "Помилка отримання списку користувачів\n");
        return;
    }

    // Разбиение ответа на строки (пользователи)
    char *line = strtok(response, "\n");
    while (line != NULL) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(line);

        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_widget_show_all(row);

        // Связываем имя пользователя с строкой списка
        g_object_set_data(G_OBJECT(row), "username", g_strdup(line));

        gtk_list_box_insert(GTK_LIST_BOX(users_list), row, -1);

        line = strtok(NULL, "\n");
    }
}

void on_login_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entries[1]));

    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Ім'я користувача та пароль не повинні бути порожніми!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    char response[256];
    if (send_to_server("LOGIN", username, password, "", response, sizeof(response)) == 0) {
        if (strcmp(response, "OK") == 0) {
            // Сохраняем имя пользователя
            strncpy(current_user, username, sizeof(current_user) - 1);
            // Открываем окно чата
            create_chat_window();
            // Закрываем окно авторизации
            GtkWidget *parent_window = gtk_widget_get_toplevel(widget);
            gtk_widget_hide(parent_window);
        } else if (strcmp(response, "USER_NOT_FOUND") == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                       "Користувача з таким ім'ям не знайдено!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else if (strcmp(response, "INVALID") == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                       "Неправильний пароль!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                       "Помилка входу. Спробуйте ще раз.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Помилка підключення до сервера!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void load_chat_messages(GtkWidget *chat_container, const char *current_user, const char *selected_user) {
    char response[2048] = {0};
    if (send_to_server("GET_MESSAGES", current_user, selected_user, "", response, sizeof(response)) == 0) {
        // Очистка контейнера сообщений
        GList *children = gtk_container_get_children(GTK_CONTAINER(chat_container));
        for (GList *child = children; child != NULL; child = child->next) {
            gtk_widget_destroy(GTK_WIDGET(child->data));
        }
        g_list_free(children);

        // Разбить ответ сервера на строки
        char *line = strtok(response, "\n");
        while (line != NULL) {
            int message_id;
            char sender[64], time[64], message[1024];

            // Извлекаем данные, включая message_id
            if (sscanf(line, "%d|%63[^|]|%63[^|]|%1023[^\n]", &message_id, sender, time, message) == 4) {
                add_message_to_chat(chat_container, sender, time, message, message_id);
            } else {
                fprintf(stderr, "Помилка парсингу повідомлення: %s\n", line);
            }
            line = strtok(NULL, "\n");
        }
    } else {
        fprintf(stderr, "Помилка отримання повідомлень із сервера.\n");
    }
}

gboolean update_chat_window(gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
    GtkWidget *chat_container = g_object_get_data(G_OBJECT(window), "chat_container");
    GtkAdjustment *adjustment = g_object_get_data(G_OBJECT(window), "chat_adjustment");
    const char *current_user = g_object_get_data(G_OBJECT(window), "current_user");
    const char *selected_user = g_object_get_data(G_OBJECT(window), "selected_user");

    if (selected_user && strlen(selected_user) > 0) {
        load_chat_messages(chat_container, current_user, selected_user);

        // Прокрутка вниз
        gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
    }

    return TRUE; // Возвращаем TRUE для повторного вызова таймера
}

void on_user_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data) {
    if (!row) return;

    GtkWidget *window = GTK_WIDGET(data);
    GtkWidget *text_view = g_object_get_data(G_OBJECT(window), "text_view");
    const char *current_user = g_object_get_data(G_OBJECT(window), "current_user");

    // Получаем имя выбранного пользователя
    const char *selected_user = gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(row))));
    g_object_set_data(G_OBJECT(window), "selected_user", (gpointer)selected_user);

    printf("Вибраний користувач: %s\n", selected_user);

    // Загрузка сообщений для выбранного пользователя
    load_chat_messages(text_view, current_user, selected_user);
}

// Обработчик для кнопки "Найти пользователя"
void on_search_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *entry_search = (GtkWidget *)data;
    const char *query = gtk_entry_get_text(GTK_ENTRY(entry_search));

    if (strlen(query) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Поле пошуку повинно бути порожнім!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    char response[256];
    if (send_to_server("SEARCH", query, "", "", response, sizeof(response)) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                   "Знайдені користувачі:\n%s", response);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Помилка пошуку користувачів!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void on_send_message_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
    GtkWidget *entry_message = g_object_get_data(G_OBJECT(window), "entry_message");
    GtkWidget *text_view = g_object_get_data(G_OBJECT(window), "text_view");
    const char *current_user = g_object_get_data(G_OBJECT(window), "current_user");
    const char *selected_user = g_object_get_data(G_OBJECT(window), "selected_user");

    if (!selected_user || strlen(selected_user) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Виберіть користувача, щоб надіслати повідомлення.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    const char *message = gtk_entry_get_text(GTK_ENTRY(entry_message));

    if (strlen(message) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Повідомлення не може бути порожнім!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Отправляем сообщение на сервер
    char response[256];
    if (send_to_server("SEND_MESSAGE", current_user, selected_user, message, response, sizeof(response)) == 0) {
        gtk_entry_set_text(GTK_ENTRY(entry_message), ""); // Очистка поля ввода
        load_chat_messages(text_view, current_user, selected_user); // Обновляем чат
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Помилка надсилання повідомлення.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}