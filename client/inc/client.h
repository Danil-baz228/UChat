// client.h
#ifndef CLIENT_H
#define CLIENT_H

#include <gtk/gtk.h>

// Объявления функций
int send_to_server(const char *command, const char *username, const char *password);
void create_chat_window();
void on_register_clicked(GtkWidget *widget, gpointer data);
void on_login_clicked(GtkWidget *widget, gpointer data);

#endif // CLIENT_H
