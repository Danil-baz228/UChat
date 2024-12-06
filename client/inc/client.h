#ifndef CLIENT_H
#define CLIENT_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

extern char current_user[64];
extern GtkTextBuffer *chat_buffer;
static int chat_window_x = 0;
static int chat_window_y = 0;

// Функции из chat.c
void create_chat_window();
void on_logout_clicked(GtkButton *button, gpointer user_data);
void on_window_destroy(GtkWidget *widget, gpointer user_data);
void add_message_to_chat(GtkWidget *chat_container, const char *sender, const char *time, const char *message, int message_id);
void *listen_to_server(void *data);

void on_edit_message_clicked(GtkButton *button, gpointer user_data);
void on_delete_message_clicked(GtkButton *button, gpointer user_data);
// client.c
const char *get_system_theme();
int send_to_server(const char *command, const char *arg1, const char *arg2, const char *arg3, char *response, size_t response_size);
void set_theme(GtkCssProvider *provider, const char *theme);

// handlers.c
gboolean update_chat_window(gpointer data);
void scroll_to_bottom(GtkTextView *text_view);
void on_register_clicked(GtkWidget *widget, gpointer data);
void load_users_list(GtkWidget *users_list);
void on_login_clicked(GtkWidget *widget, gpointer data);
void load_chat_messages(GtkWidget *chat_container, const char *current_user, const char *selected_user);
void on_user_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data);
void on_search_clicked(GtkWidget *widget, gpointer data);
void on_send_message_clicked(GtkWidget *widget, gpointer data);

//language.c
void on_switch_language_clicked(GtkMenuItem *menuitem, gpointer user_data);
void update_text_labels(gpointer user_data);

//settings.c
void on_toggle_notifications(GtkMenuItem *menuitem, gpointer user_data);
void on_change_theme(GtkMenuItem *menuitem, gpointer user_data);
void on_about_clicked(GtkMenuItem *menuitem, gpointer user_data);
const char *get_system_theme_chat();

//window_manager.c
const char *get_system_theme_chat();
void set_main_window(GtkWidget *window);
void set_chat_window(GtkWidget *window);
void set_sticker_window(GtkWidget *window);

void add_search_bar(GtkWidget *main_vertical_box, GtkWidget *users_list);

void create_login_window();
void close_registration_window(GtkWidget *window);

#endif // CLIENT_H

