#include "../inc/client.h"
#include "stickers.c"
#include "style_chat.c"

GtkTextBuffer *chat_buffer = NULL;
GtkWidget *main_vertical_box;

char current_language[3];
char current_language[3] = "UA";

void on_logout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);
    gtk_widget_hide(window);
    create_login_window();
}

void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    exit(0);
}

void create_chat_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Чат");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    GtkWidget *main_vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(main_vertical_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_vertical_box);

    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(header_box), TRUE);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), header_box, FALSE, FALSE, 0);

    char user_label_text[128];
    snprintf(user_label_text, sizeof(user_label_text), "Користувач: %s", current_user);
    GtkWidget *user_label = gtk_label_new(user_label_text);

    gtk_widget_set_halign(user_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(header_box), user_label, TRUE, TRUE, 15);

    GtkWidget *logout_and_settings_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(header_box), logout_and_settings_box, FALSE, FALSE, 0);

    gtk_box_set_homogeneous(GTK_BOX(header_box), FALSE);
	gtk_box_set_homogeneous(GTK_BOX(logout_and_settings_box), FALSE);

    GtkWidget *settings_button = gtk_menu_button_new();
    GtkWidget *settings_icon = gtk_image_new_from_icon_name("emblem-system", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(settings_button), settings_icon);
    gtk_box_pack_start(GTK_BOX(logout_and_settings_box), settings_button, FALSE, FALSE, 0);

    gtk_widget_set_margin_start(logout_and_settings_box, 350);

    GtkWidget *settings_menu = gtk_menu_new();

    GtkWidget *language_item = gtk_menu_item_new_with_label("Змінити мову: UA");
    g_signal_connect(language_item, "activate", G_CALLBACK(on_switch_language_clicked), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), language_item);

    GtkWidget *logout_item = gtk_menu_item_new_with_label("Вийти");
    g_signal_connect(logout_item, "activate", G_CALLBACK(on_logout_clicked), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), logout_item);

    gtk_menu_button_set_popup(GTK_MENU_BUTTON(settings_button), settings_menu);
    gtk_widget_show_all(settings_menu);

    GtkWidget *gear_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), gear_button_box, FALSE, FALSE, 0);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), main_box, TRUE, TRUE, 0);

	GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_vexpand(left_panel, TRUE);
	gtk_box_pack_start(GTK_BOX(main_box), left_panel, FALSE, TRUE, 0);

    GtkWidget *top_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(left_panel), top_container, FALSE, FALSE, 0);
    gtk_widget_set_margin_top(top_container, 0);
    gtk_widget_set_margin_bottom(left_panel, 0);

	GtkWidget *users_list = gtk_list_box_new();
	gtk_widget_set_vexpand(users_list, TRUE);

	add_search_bar(top_container, users_list);

	GtkWidget *scrolled_users = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_users), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled_users, 150, -1);

	gtk_container_add(GTK_CONTAINER(scrolled_users), users_list);
	gtk_box_pack_start(GTK_BOX(left_panel), scrolled_users, TRUE, TRUE, 0);

    g_signal_connect(users_list, "row-selected", G_CALLBACK(on_user_selected), window);

    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), right_box, TRUE, TRUE, 0);

    GtkWidget *chat_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_vexpand(chat_container, TRUE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), chat_container);
    gtk_box_pack_start(GTK_BOX(right_box), scrolled_window, TRUE, TRUE, 0);

    g_object_set_data(G_OBJECT(window), "chat_container", chat_container);
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    g_object_set_data(G_OBJECT(window), "chat_adjustment", adjustment);

    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(right_box), message_box, FALSE, FALSE, 0);

    GtkWidget *entry_message = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Введіть повідомлення");
    gtk_widget_set_hexpand(entry_message, TRUE);
    gtk_box_pack_start(GTK_BOX(message_box), entry_message, TRUE, TRUE, 0);

    GtkWidget *button_send = gtk_button_new_with_label("Надіслати");
    gtk_box_pack_start(GTK_BOX(message_box), button_send, FALSE, FALSE, 0);

    GtkWidget *sticker_button = gtk_button_new_with_label("🙂");
    gtk_box_pack_start(GTK_BOX(message_box), sticker_button, FALSE, FALSE, 0);

    g_object_set_data(G_OBJECT(window), "send_button", button_send);
    g_object_set_data(G_OBJECT(window), "sticker_button", sticker_button);

    g_object_set_data(G_OBJECT(window), "window", window);
    g_object_set_data(G_OBJECT(window), "user_label", user_label);
    g_object_set_data(G_OBJECT(window), "language_item", language_item);
    g_object_set_data(G_OBJECT(window), "logout_item", logout_item);

    GtkWidget *sticker_popover = gtk_popover_new(sticker_button);
    GtkWidget *sticker_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sticker_scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(sticker_scrolled_window, 200, 200);
    gtk_container_add(GTK_CONTAINER(sticker_popover), sticker_scrolled_window);

    GtkWidget *sticker_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(sticker_scrolled_window), sticker_grid);

    int num_stickers = sizeof(stickers) / sizeof(stickers[0]);

    for (int i = 0; i < num_stickers; i++) {
        GtkWidget *sticker_item_button = gtk_button_new_with_label(stickers[i]);
        g_signal_connect(sticker_item_button, "clicked", G_CALLBACK(on_sticker_selected), entry_message);
        gtk_grid_attach(GTK_GRID(sticker_grid), sticker_item_button, i % 3, i / 3, 1, 1);
    }

    g_signal_connect(sticker_button, "clicked", G_CALLBACK(on_sticker_button_clicked), sticker_popover);

    GtkCssProvider *provider = gtk_css_provider_new();
    const char *current_theme = get_system_theme_chat();
    set_chat_theme(provider, current_theme);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(window);
    load_users_list(users_list);

    g_timeout_add(2000, (GSourceFunc)update_chat_window, window);

    g_object_set_data(G_OBJECT(window), "entry_message", entry_message);
    g_object_set_data(G_OBJECT(window), "users_list", users_list);
    g_object_set_data(G_OBJECT(window), "current_user", current_user);
    g_object_set_data(G_OBJECT(window), "selected_user", NULL);

    g_signal_connect(button_send, "clicked", G_CALLBACK(on_send_message_clicked), window);
}

const char *get_system_theme_chat() {
    GSettings *settings = g_settings_new("org.gnome.desktop.interface");
    if (!settings) {
        return "light";
    }

    gchar *theme = g_settings_get_string(settings, "gtk-theme");
    const char *result = "light";

    if (theme) {
        if (g_str_has_suffix(theme, "-dark")) {
            result = "dark";
        }
        g_free(theme);
    }

    g_object_unref(settings);
    return result;
}

void on_change_theme(GtkMenuItem *menuitem, gpointer user_data) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *current_theme = get_system_theme_chat();
    const char *new_theme = (mx_strcmp(current_theme, "dark") == 0) ? "light" : "dark";
    set_chat_theme(provider, new_theme);
    g_object_unref(provider);
}





