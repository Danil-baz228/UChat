#include "../inc/client.h"

void on_search_entry_changed(GtkSearchEntry *entry, gpointer user_data) {
    GtkWidget *users_list = GTK_WIDGET(user_data);
    const char *search_text = gtk_entry_get_text(GTK_ENTRY(entry));

    GList *children = gtk_container_get_children(GTK_CONTAINER(users_list));
    for (GList *l = children; l != NULL; l = l->next) {
        GtkWidget *row = GTK_WIDGET(l->data);
        const char *label_text = gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(row))));

        if (mx_strrstr(g_ascii_strdown(label_text, -1), g_ascii_strdown(search_text, -1)) != NULL) {
            gtk_widget_show(row);
        } else {
            gtk_widget_hide(row);
        }
    }

    g_list_free(children);
}

void add_search_bar(GtkWidget *main_vertical_box, GtkWidget *users_list) {
    GtkWidget *search_bar = gtk_search_entry_new();

    g_object_set_data(G_OBJECT(gtk_widget_get_toplevel(main_vertical_box)), "search_entry", search_bar);

    gtk_entry_set_placeholder_text(GTK_ENTRY(search_bar), "Пошук контактів...");
    gtk_box_pack_start(GTK_BOX(main_vertical_box), search_bar, FALSE, FALSE, 0);

    GtkWidget *window = gtk_widget_get_toplevel(main_vertical_box);
    g_object_set_data(G_OBJECT(window), "search_entry", search_bar);

    g_signal_connect(search_bar, "search-changed", G_CALLBACK(on_search_entry_changed), users_list);
}

