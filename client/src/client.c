#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090

// Функция для отправки данных на сервер
int send_to_server(const char *command, const char *username, const char *password) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sock);
        return -1;
    }

    // Формируем сообщение
    char message[256];
    snprintf(message, sizeof(message), "%s %s %s", command, username, password);
    send(sock, message, strlen(message), 0);

    // Получаем ответ от сервера
    char response[256];
    memset(response, 0, sizeof(response));
    read(sock, response, sizeof(response) - 1);
    close(sock);

    printf("Server response: %s\n", response);

    return strcmp(response, "OK") == 0 ? 0 : -1;
}

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

// Обработчик для кнопки "Зарегистрироваться"
void on_register_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entries[1]));

    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Имя пользователя и пароль не должны быть пустыми!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (send_to_server("REGISTER", username, password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                   "Регистрация прошла успешно!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Ошибка регистрации!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Обработчик для кнопки "Войти"
void on_login_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entries[1]));

    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Имя пользователя и пароль не должны быть пустыми!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (send_to_server("LOGIN", username, password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                   "Вход выполнен успешно!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Открываем окно чата
        create_chat_window();
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Неверное имя пользователя или пароль!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Главное окно
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Клиент");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Сетка для размещения виджетов
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Поля ввода
    GtkWidget *label_username = gtk_label_new("Имя пользователя:");
    GtkWidget *entry_username = gtk_entry_new();

    GtkWidget *label_password = gtk_label_new("Пароль:");
    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);

    // Кнопки
    GtkWidget *button_register = gtk_button_new_with_label("Зарегистрироваться");
    GtkWidget *button_login = gtk_button_new_with_label("Войти");

    // Привязка обработчиков к кнопкам
    GtkWidget *entries[2] = {entry_username, entry_password};
    g_signal_connect(button_register, "clicked", G_CALLBACK(on_register_clicked), entries);
    g_signal_connect(button_login, "clicked", G_CALLBACK(on_login_clicked), entries);

    // Добавление виджетов в сетку
    gtk_grid_attach(GTK_GRID(grid), label_username, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_username, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), label_password, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_password, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), button_register, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), button_login, 0, 3, 2, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
