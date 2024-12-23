#include "../inc/client.h"
#include "style.c"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090

char current_user[64] = "";

int send_to_server(const char *command, const char *arg1, const char *arg2, const char *arg3, char *response, size_t response_size) {
    if (!command || !arg1 || !arg2 || !arg3 || !response) {
        mx_printerr("send_to_server: invalid arguments\n");
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        mx_printerr("Socket creation failed\n");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        mx_printerr("Connection to server failed\n");
        close(sock);
        return -1;
    }

    char message[256];
    snprintf(message, sizeof(message), "%s %s %s %s", command, arg1, arg2, arg3);
    send(sock, message, mx_strlen(message), 0);

    mx_memset(response, 0, response_size);
    read(sock, response, response_size - 1);
    close(sock);

    return 0;
}

const char *get_system_theme() {
    GSettings *settings = g_settings_new("org.gnome.desktop.interface");
    if (!settings) {
        return "light";
    }

    gchar *theme = g_settings_get_string(settings, "gtk-theme");
    const char *result = "light";

    if (theme) {
        if (g_str_has_suffix(theme, "-dark")) {
            result = "dark";
        } else {
            result = "light";
        }
        g_free(theme);
    }

    g_object_unref(settings);
    return result;
}

void set_theme(GtkCssProvider *provider, const char *theme) {
    if (mx_strcmp(theme, "dark") == 0) {
        gtk_css_provider_load_from_data(provider, dark_css, -1, NULL);
    } else {
        gtk_css_provider_load_from_data(provider, light_css, -1, NULL);
    }
}

void close_login_window(GtkWidget *window) {
    GtkWidget *parent_window = gtk_widget_get_toplevel(window);
    gtk_widget_hide(parent_window);
}

void close_registration_window(GtkWidget *window) {
    GtkWidget *parent_window = gtk_widget_get_toplevel(window);
    gtk_widget_hide(parent_window);

    create_login_window();
}

void create_registration_window(GtkWidget *login_window) {
    close_login_window(login_window);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Реєстрація");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(exit), NULL);

    const char *current_theme = get_system_theme();

    GtkCssProvider *provider = gtk_css_provider_new();
    set_theme(provider, current_theme);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *label_username = gtk_label_new("Ім'я користувача:");
    GtkWidget *entry_username = gtk_entry_new();
    gtk_widget_set_margin_start(label_username, 5);
    gtk_widget_set_margin_end(label_username, 5);
    gtk_widget_set_margin_top(entry_username, 5);
    gtk_widget_set_margin_bottom(entry_username, 10);

    GtkWidget *label_password = gtk_label_new("Пароль:");
    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_margin_start(label_password, 5);
    gtk_widget_set_margin_end(label_password, 5);
    gtk_widget_set_margin_top(entry_password, 5);
    gtk_widget_set_margin_bottom(entry_password, 10);

    GtkWidget *button_register = gtk_button_new_with_label("Зареєструватись");
    gtk_widget_set_margin_top(button_register, 15);
    gtk_widget_set_margin_bottom(button_register, 10);

    GtkWidget *button_back = gtk_button_new_with_label("Назад");
    gtk_widget_set_margin_top(button_back, 5);

    GtkWidget *entries[2] = {entry_username, entry_password};
    g_signal_connect(button_register, "clicked", G_CALLBACK(on_register_clicked), entries);
    g_signal_connect(button_back, "clicked", G_CALLBACK(close_registration_window), window); 

    gtk_grid_attach(GTK_GRID(grid), label_username, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_username, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), label_password, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_password, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), button_register, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), button_back, 0, 3, 2, 1);

    gtk_widget_show_all(window);
    gtk_main();
}

void create_login_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Авторизація");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(exit), NULL);

    const char *current_theme = get_system_theme();

    GtkCssProvider *provider = gtk_css_provider_new();
    set_theme(provider, current_theme);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *label_username = gtk_label_new("Ім'я користувача:");
    GtkWidget *entry_username = gtk_entry_new();
    gtk_widget_set_margin_start(label_username, 5);
    gtk_widget_set_margin_end(label_username, 5);
    gtk_widget_set_margin_top(entry_username, 5);
    gtk_widget_set_margin_bottom(entry_username, 10);

    GtkWidget *label_password = gtk_label_new("Пароль:");
    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_margin_start(label_password, 5);
    gtk_widget_set_margin_end(label_password, 5);
    gtk_widget_set_margin_top(entry_password, 5);
    gtk_widget_set_margin_bottom(entry_password, 10);

    GtkWidget *button_login = gtk_button_new_with_label("Увійти");
    gtk_widget_set_margin_top(button_login, 10);

    GtkWidget *label_register_prompt = gtk_label_new("Немає облікового запису? Зареєструйся");
    gtk_widget_set_margin_top(label_register_prompt, 10);

    GtkWidget *button_register = gtk_button_new_with_label("Почати реєстрацію");
    gtk_widget_set_margin_top(button_register, 5);

    GtkWidget *entries[2] = {entry_username, entry_password};
    g_signal_connect(button_login, "clicked", G_CALLBACK(on_login_clicked), entries);
    g_signal_connect(button_register, "clicked", G_CALLBACK(create_registration_window), window); 

    gtk_grid_attach(GTK_GRID(grid), label_username, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_username, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), label_password, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_password, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), button_login, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), label_register_prompt, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), button_register, 0, 4, 2, 1);

    gtk_widget_show_all(window);
    gtk_main();
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    create_login_window();

    return 0;
}
