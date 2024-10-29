#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;
GtkTextBuffer *text_buffer;
GtkWidget *login_window, *client_list;  // Окна для входа и панель для списка клиентов
const char *username;    // Переменная для хранения имени пользователя
GtkWidget *register_window, *chat_window; // Окна для регистрации и чата

// Прототипы функций
void create_chat_window();
void create_login_window();
void create_register_window();
void update_client_list(const gchar *username, gboolean is_connected);
void on_send_button_clicked(GtkButton *button, gpointer user_data);
void on_emoji_selected(GtkButton *button, gpointer entry);  // Обработчик для вставки эмодзи
void open_emoji_popover(GtkWidget *button, gpointer entry);


void create_chat_window() {
    GtkWidget *vbox, *hbox, *text_view, *entry, *send_button, *emoji_button;

    chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(chat_window), "Chat");
    gtk_window_set_default_size(GTK_WINDOW(chat_window), 500, 400);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(chat_window), hbox);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

    text_view = gtk_text_view_new();
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    send_button = gtk_button_new_with_label("Send");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    // Кнопка для открытия выпадающего списка со смайликами
    emoji_button = gtk_button_new_with_label("😊");
    g_signal_connect(emoji_button, "clicked", G_CALLBACK(open_emoji_popover), entry);
    gtk_box_pack_start(GTK_BOX(vbox), emoji_button, FALSE, FALSE, 0);

    // Панель для отображения списка клиентов
    client_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *client_list_label = gtk_label_new("Connected Users:");
    gtk_box_pack_start(GTK_BOX(client_list), client_list_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), client_list, FALSE, FALSE, 0);

    g_signal_connect(chat_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(chat_window);
}

void open_emoji_popover(GtkWidget *button, gpointer entry) {
    GtkWidget *popover = gtk_popover_new(button);
    GtkWidget *emoji_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(popover), emoji_box);


    // Массив смайликов для выбора
    const char *emojis[] = {"😀", "😂", "❤️", "👍", "🎉", "😊", "😎", "🤔", "😢", "🎈"};

    for (int i = 0; i < sizeof(emojis) / sizeof(emojis[0]); i++) {
        GtkWidget *emoji_button = gtk_button_new_with_label(emojis[i]);
        g_signal_connect(emoji_button, "clicked", G_CALLBACK(on_emoji_selected), entry);
        gtk_box_pack_start(GTK_BOX(emoji_box), emoji_button, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(emoji_box);
    gtk_popover_popup(GTK_POPOVER(popover));
}

// Функция для получения сообщений от сервера
void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received from server: %s\n", buffer);

            if (strncmp(buffer, "USER_CONNECTED:", 15) == 0) {
                const char *username = buffer + 15;
                printf("User connected: %s\n", username);
                update_client_list(username, TRUE);
            } else if (strncmp(buffer, "USER_DISCONNECTED:", 18) == 0) {
                const char *username = buffer + 18;
                printf("User disconnected: %s\n", username);
                update_client_list(username, FALSE);
            } else {
                if (g_utf8_validate(buffer, bytes_read, NULL)) {
                    gtk_text_buffer_insert_at_cursor(text_buffer, buffer, -1);
                    gtk_text_buffer_insert_at_cursor(text_buffer, "\n", -1);
                } else {
                    fprintf(stderr, "Received invalid UTF-8 message: %.*s\n", bytes_read, buffer);
                }
            }
        } else {
            break;
        }
    }
    return NULL;
}

// Обработчик выбора эмодзи
void on_emoji_selected(GtkButton *button, gpointer entry) {
    const gchar *emoji = gtk_button_get_label(button);
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
    gchar *new_text = g_strconcat(current_text, emoji, NULL);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}

// Функция отправки сообщений
void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    const gchar *message = gtk_entry_get_text(GTK_ENTRY(user_data));
    if (strlen(message) > 0) {
        send(sock, message, strlen(message), 0);
        char display_message[BUFFER_SIZE];
        snprintf(display_message, sizeof(display_message), "You: %s", message);

        if (g_utf8_validate(display_message, -1, NULL)) {
            gtk_text_buffer_insert_at_cursor(text_buffer, display_message, -1);
            gtk_text_buffer_insert_at_cursor(text_buffer, "\n", -1);
        } else {
            fprintf(stderr, "Invalid UTF-8 message: %s\n", display_message);
        }
        gtk_entry_set_text(GTK_ENTRY(user_data), "");
    }
}

// Обновление списка клиентов
void update_client_list(const gchar *username, gboolean is_connected) {
    printf("Updating client list for user: %s, connected: %d\n", username, is_connected);
    if (is_connected) {
        GtkWidget *row = gtk_label_new(username);
        gtk_box_pack_start(GTK_BOX(client_list), row, FALSE, FALSE, 0);
    } else {
        GList *children = gtk_container_get_children(GTK_CONTAINER(client_list));
        for (GList *l = children; l != NULL; l = l->next) {
            GtkWidget *label = GTK_WIDGET(l->data);
            const gchar *label_text = gtk_label_get_text(GTK_LABEL(label));
            if (strcmp(label_text, username) == 0) {
                gtk_widget_destroy(label);
                break;
            }
        }
        g_list_free(children);
    }
    gtk_widget_show_all(client_list);
}

// Вход пользователя
void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    username = gtk_entry_get_text(GTK_ENTRY(user_data));
    send(sock, username, strlen(username), 0);
    gtk_widget_hide(login_window);
    create_chat_window();
}

void on_register_button_clicked(GtkButton *button, gpointer user_data) {
    gtk_widget_hide(login_window);
    create_register_window();
}
// Создание окна входа
void create_login_window() {
    GtkWidget *vbox;
    GtkWidget *entry;
    GtkWidget *login_button;
    GtkWidget *register_button;

    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 300, 150);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(login_window), vbox);

    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter username");
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0);

    login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(vbox), login_button, TRUE, TRUE, 0);

    register_button = gtk_button_new_with_label("Register");
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), register_button, TRUE, TRUE, 0);

    g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(login_window);
}



void on_confirm_registration_button_clicked(GtkButton *button, gpointer user_data) {
    // Получаем введенные данные
    GtkWidget **entries = (GtkWidget **)user_data;
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    const gchar *confirm_password = gtk_entry_get_text(GTK_ENTRY(entries[2]));

    // Проверяем, совпадают ли пароли
    if (strcmp(password, confirm_password) != 0) {
        g_print("Passwords do not match\n");
        return;
    }

    // Логика для регистрации пользователя, отправка данных на сервер
    g_print("Registered username: %s\n", username);

    // После регистрации скрываем окно регистрации и показываем окно логина
    gtk_widget_hide(register_window);
    create_login_window();
}

void on_back_to_login_clicked(GtkButton *button, gpointer user_data) {
    gtk_widget_hide(register_window);  // Скрыть окно регистрации
    create_login_window();             // Показать окно логина
}


void create_register_window() {
    GtkWidget *vbox;
    GtkWidget *username_entry, *password_entry, *confirm_password_entry;
    GtkWidget *confirm_button, *back_button;

    register_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(register_window), "Register");
    gtk_window_set_default_size(GTK_WINDOW(register_window), 300, 200);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(register_window), vbox);

    username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter username");
    gtk_box_pack_start(GTK_BOX(vbox), username_entry, TRUE, TRUE, 0);

    password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);  // Скрыть ввод пароля
    gtk_box_pack_start(GTK_BOX(vbox), password_entry, TRUE, TRUE, 0);

    confirm_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(confirm_password_entry), "Confirm password");
    gtk_entry_set_visibility(GTK_ENTRY(confirm_password_entry), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), confirm_password_entry, TRUE, TRUE, 0);

    // Массив для передачи нескольких полей в обработчик
    GtkWidget *entries[] = { username_entry, password_entry, confirm_password_entry };

    confirm_button = gtk_button_new_with_label("Confirm");
    g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_confirm_registration_button_clicked), entries);
    gtk_box_pack_start(GTK_BOX(vbox), confirm_button, TRUE, TRUE, 0);

    back_button = gtk_button_new_with_label("Back to Login");
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_to_login_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), back_button, TRUE, TRUE, 0);

    g_signal_connect(register_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(register_window);
}


int main(int argc, char **argv) {
    struct sockaddr_in server_addr;

    gtk_init(&argc, &argv);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    create_login_window();

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    gtk_main();

    close(sock);
    return 0;
}