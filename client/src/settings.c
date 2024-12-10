#include "../inc/client.h"

extern GtkWidget *main_vertical_box;
extern char current_language[3];

void on_switch_language_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    if (mx_strcmp(current_language, "UA") == 0) {
        current_language[0] = 'E';
        current_language[1] = 'N';
    } else {
        current_language[0] = 'U';
        current_language[1] = 'A';
    }
    update_text_labels(user_data);
}

void update_text_labels(gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);
    gtk_window_set_title(GTK_WINDOW(window), mx_strcmp(current_language, "UA") == 0 ? "Чат" : "Chat");

    GtkWidget *search_entry = gtk_search_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Пошук контактів...");
    gtk_box_pack_start(GTK_BOX(main_vertical_box), search_entry, FALSE, FALSE, 0);

    GtkWidget *logout_button = g_object_get_data(G_OBJECT(window), "logout_button");
    GtkWidget *send_button = g_object_get_data(G_OBJECT(window), "send_button");
    GtkWidget *sticker_button = g_object_get_data(G_OBJECT(window), "sticker_button");
    GtkWidget *user_label = g_object_get_data(G_OBJECT(window), "user_label");
    GtkWidget *entry_message = g_object_get_data(G_OBJECT(window), "entry_message");

    GtkWidget *theme_item = g_object_get_data(G_OBJECT(window), "theme_item");
    GtkWidget *about_item = g_object_get_data(G_OBJECT(window), "about_item");
    GtkWidget *language_item = g_object_get_data(G_OBJECT(window), "language_item");
    GtkWidget *notifications_item = g_object_get_data(G_OBJECT(window), "notifications_item");
    GtkWidget *logout_item = g_object_get_data(G_OBJECT(window), "logout_item");

    if (mx_strcmp(current_language, "UA") == 0) {
        gtk_menu_item_set_label(GTK_MENU_ITEM(language_item), "Змінити мову: UA");
        gtk_menu_item_set_label(GTK_MENU_ITEM(logout_item), "Вийти");
    } else {
        gtk_menu_item_set_label(GTK_MENU_ITEM(language_item), "Switch Language: ENG");
        gtk_menu_item_set_label(GTK_MENU_ITEM(logout_item), "Logout");
    }
    GtkWidget *search_bar = g_object_get_data(G_OBJECT(window), "search_entry");
    if (search_bar) {
        gtk_entry_set_placeholder_text(GTK_ENTRY(search_bar),
            mx_strcmp(current_language, "UA") == 0 ? "Пошук контактів..." : "Search contacts...");
    }
    if (mx_strcmp(current_language, "UA") == 0) {
        gtk_button_set_label(GTK_BUTTON(logout_button), "Вийти");
        gtk_button_set_label(GTK_BUTTON(send_button), "Надіслати");
        gtk_button_set_label(GTK_BUTTON(sticker_button), "🙂");
        if (user_label) {
            char user_label_text[128];
            snprintf(user_label_text, sizeof(user_label_text), "Користувач: %s", current_user);
            gtk_label_set_text(GTK_LABEL(user_label), user_label_text);
        }
        if (entry_message) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Введіть повідомлення");
        }
    } else {
        gtk_button_set_label(GTK_BUTTON(logout_button), "Logout");
        gtk_button_set_label(GTK_BUTTON(send_button), "Send");
        gtk_button_set_label(GTK_BUTTON(sticker_button), "🙂");
        if (user_label) {
            char user_label_text[128];
            snprintf(user_label_text, sizeof(user_label_text), "User: %s", current_user);
            gtk_label_set_text(GTK_LABEL(user_label), user_label_text);
        }
        if (entry_message) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Enter message");
        }
    }
}
