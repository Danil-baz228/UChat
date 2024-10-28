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
const char *username; // Переменная для хранения имени пользователя
GtkWidget *login_window; // Окно логина

// Прототипы функций
void create_chat_window();

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            // Отображаем сообщение от другого пользователя
            gtk_text_buffer_insert_at_cursor(text_buffer, buffer, -1);
            gtk_text_buffer_insert_at_cursor(text_buffer, "\n", -1); // Добавляем новую строку
        } else {
            break; // Соединение закрыто
        }
    }
    return NULL;
}

void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    const gchar *message = gtk_entry_get_text(GTK_ENTRY(user_data));
    if (strlen(message) > 0) {
        // Отправляем сообщение в сыром виде, без имени пользователя
        send(sock, message, strlen(message), 0);

        // Отображаем отправленное сообщение в окне чата
        char display_message[BUFFER_SIZE];
        snprintf(display_message, sizeof(display_message), "You: %s", message);
        gtk_text_buffer_insert_at_cursor(text_buffer, display_message, -1);
        gtk_text_buffer_insert_at_cursor(text_buffer, "\n", -1); // Добавляем новую строку
        gtk_entry_set_text(GTK_ENTRY(user_data), ""); // Очищаем поле ввода
    }
}


void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    username = gtk_entry_get_text(GTK_ENTRY(user_data)); // Сохраняем имя пользователя

    // Отправляем имя пользователя на сервер для аутентификации
    send(sock, username, strlen(username), 0);

    // Скрываем окно логина
    gtk_widget_hide(login_window);

    // Создаем окно чата
    create_chat_window();
}

void create_login_window() {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *entry;
    GtkWidget *login_button;

    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 300, 100);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(login_window), vbox);

    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter username");
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0);

    login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(vbox), login_button, TRUE, TRUE, 0);

    g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(login_window);
}

void create_chat_window() {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *text_view;
    GtkWidget *entry;
    GtkWidget *send_button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400); // Увеличиваем размер окна

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    text_view = gtk_text_view_new();
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    send_button = gtk_button_new_with_label("Send");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    struct sockaddr_in serv_addr;

    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Преобразование IPv4 и IPv6 адресов из текстового формата в бинарный
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    // Инициализация GTK
    gtk_init(&argc, &argv);
    create_login_window();
    pthread_t tid;
    pthread_create(&tid, NULL, receive_messages, NULL); // Запускаем поток для получения сообщений
    pthread_detach(tid); // Отключаем поток, чтобы ресурсы освобождались при его завершении
    gtk_main();

    close(sock);
    return 0;
}
