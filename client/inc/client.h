#ifndef CLIENT_H
#define CLIENT_H

#include <gtk/gtk.h>
#include <gio/gio.h>

extern char current_user[64];
extern GtkTextBuffer *chat_buffer;
static int chat_window_x = 0;
static int chat_window_y = 0;

void set_theme(GtkCssProvider *provider, const char *theme);


// Объявления функций
int send_to_server(const char *command, const char *arg1, const char *arg2, const char *arg3, char *response, size_t response_size);
void create_chat_window();
void on_register_clicked(GtkWidget *widget, gpointer data);
void on_login_clicked(GtkWidget *widget, gpointer data);
void on_search_clicked(GtkWidget *widget, gpointer data);
void on_send_message_clicked(GtkWidget *widget, gpointer data);
void load_chat_messages(GtkWidget *text_view, const char *current_user, const char *selected_user);

void load_users_list(GtkWidget *users_list);
void on_user_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data);
void create_login_window();

gboolean update_chat_window(gpointer data);
void on_sticker_button_clicked(GtkButton *button, gpointer user_data);
void on_sticker_selected(GtkButton *button, gpointer user_data);
void hide_sticker_window(GtkWidget *sticker_window, gpointer user_data);
void update_button_position(GtkButton *button, GtkWidget *sticker_window);



void set_main_window(GtkWidget *window);
GtkWidget *get_main_window();

void set_chat_window(GtkWidget *window);
GtkWidget *get_chat_window();

void set_sticker_window(GtkWidget *window);
GtkWidget *get_sticker_window();

gboolean on_window_moved(GtkWidget *widget, GdkEvent *event, gpointer data);


int handle_register(const char *username, const char *password);
int handle_login(const char *username, const char *password);

#endif // CLIENT_H
