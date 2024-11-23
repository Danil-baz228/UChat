#ifndef CLIENT_H
#define CLIENT_H

#include <gtk/gtk.h>

extern char current_user[64];
extern GtkTextBuffer *chat_buffer;

// Объявления функций
int send_to_server(const char *command, const char *arg1, const char *arg2, const char *arg3, char *response, size_t response_size);
void create_chat_window();
void on_register_clicked(GtkWidget *widget, gpointer data);
void on_login_clicked(GtkWidget *widget, gpointer data);
void on_search_clicked(GtkWidget *widget, gpointer data);
void on_send_message_clicked(GtkWidget *widget, gpointer data);
void load_chat_messages(GtkWidget *text_view, const char *current_user, const char *selected_user);
gboolean update_chat_window(gpointer data);
void load_users_list(GtkWidget *users_list);
void on_user_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data);
void create_login_window();
gboolean update_sticker_window_position(GtkWidget *sticker_window);

GtkWidget *create_sticker_window(GtkWidget *parent_window, GtkWidget *text_view);
void show_sticker_window(GtkButton *button, gpointer user_data);

void on_sticker_selected(GtkButton *button, gpointer user_data);
void hide_sticker_window(GtkWidget *sticker_window, gpointer user_data);
void update_button_position(GtkButton *button, GtkWidget *sticker_window);
#endif // CLIENT_H
