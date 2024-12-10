#ifndef CLIENT_H
#define CLIENT_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "../../resources/libmx/inc/libmx.h"

extern char current_user[64];
extern GtkTextBuffer *chat_buffer;
static int chat_window_x = 0;
static int chat_window_y = 0;


// chat.c
void on_logout_clicked(GtkButton *button, gpointer user_data);
void on_window_destroy(GtkWidget *widget, gpointer user_data);
void create_chat_window();
const char *get_system_theme_chat();
void on_change_theme(GtkMenuItem *menuitem, gpointer user_data);

// client.c
int send_to_server(const char *command, const char *arg1, const char *arg2, const char *arg3, char *response, size_t response_size);
const char *get_system_theme();
void set_theme(GtkCssProvider *provider, const char *theme);
void close_login_window(GtkWidget *window);
void close_registration_window(GtkWidget *window);
void create_registration_window(GtkWidget *login_window);
void create_login_window();

// handlers.c
void scroll_to_bottom(GtkTextView *text_view);
void on_register_clicked(GtkWidget *widget, gpointer data);
void load_users_list(GtkWidget *users_list);
void on_login_clicked(GtkWidget *widget, gpointer data);
void load_chat_messages(GtkWidget *chat_container, const char *current_user, const char *selected_user);
gboolean update_chat_window(gpointer data);
void on_user_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data);
void on_search_clicked(GtkWidget *widget, gpointer data);
void on_send_message_clicked(GtkWidget *widget, gpointer data);

// message_client.c
void add_message_to_chat(GtkWidget *chat_container, const char *sender, const char *time, const char *message, int message_id);
void on_delete_message_clicked(GtkButton *button, gpointer user_data);
void on_edit_message_clicked(GtkButton *button, gpointer user_data);

// search.c
void on_search_entry_changed(GtkSearchEntry *entry, gpointer user_data);
void add_search_bar(GtkWidget *main_vertical_box, GtkWidget *users_list);

// settings.c
void on_switch_language_clicked(GtkMenuItem *menuitem, gpointer user_data);
void on_about_clicked(GtkMenuItem *menuitem, gpointer user_data);
void update_text_labels(gpointer user_data);

// window_manager.c
void set_main_window(GtkWidget *window);
void set_chat_window(GtkWidget *window);
void set_sticker_window(GtkWidget *window);


#endif // CLIENT_H

