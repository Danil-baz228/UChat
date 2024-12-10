#include "../inc/client.h"

extern char current_language[3];

void add_message_to_chat(GtkWidget *chat_container, const char *sender, const char *time, const char *message, int message_id) {
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(message_box, 5);
    gtk_widget_set_margin_bottom(message_box, 5);
    gtk_widget_set_margin_start(message_box, 10);
    gtk_widget_set_margin_end(message_box, 10);

    char sender_and_time[128];
    snprintf(sender_and_time, sizeof(sender_and_time), "%s [%s]", sender, time);
    GtkWidget *meta_label = gtk_label_new(sender_and_time);
    gtk_widget_set_halign(meta_label, GTK_ALIGN_START);

    GtkWidget *message_label = gtk_label_new(message);
    gtk_label_set_line_wrap(GTK_LABEL(message_label), TRUE);
    gtk_widget_set_halign(message_label, GTK_ALIGN_START);

    GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(buttons_box, GTK_ALIGN_END);

    GtkWidget *edit_button = gtk_button_new_with_label("✏️");
    gtk_widget_set_name(edit_button, "edit-button");
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_message_clicked), GINT_TO_POINTER(message_id));

    GtkWidget *delete_button = gtk_button_new_with_label("❌");
    gtk_widget_set_name(delete_button, "delete-button");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_message_clicked), GINT_TO_POINTER(message_id));

    gtk_box_pack_start(GTK_BOX(buttons_box), edit_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttons_box), delete_button, FALSE, FALSE, 0);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(content_box), message_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content_box), buttons_box, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(message_box), meta_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(message_box), content_box, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(chat_container), message_box, FALSE, FALSE, 0);

    gtk_widget_show_all(chat_container);
}

void on_delete_message_clicked(GtkButton *button, gpointer user_data) {
    int message_id = GPOINTER_TO_INT(user_data);

    char response[256];

    if (send_to_server("DELETE_MESSAGE", g_strdup_printf("%d", message_id), "", "", response, sizeof(response)) == 0) {

        if (mx_strcmp(response, "OK") == 0) {
            gtk_widget_destroy(gtk_widget_get_parent(GTK_WIDGET(button)));
        } else {
            fprintf(stderr, "Failed to delete message on server. Response: %s\n", response);
        }
    } else {
        fprintf(stderr, "Failed to send DELETE_MESSAGE command to server.\n");
    }
}

void on_edit_message_clicked(GtkButton *button, gpointer user_data) {
    int message_id = GPOINTER_TO_INT(user_data);

    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    const char *dialog_title = mx_strcmp(current_language, "UA") == 0 ? "Редагувати повідомлення" : "Edit message";
    const char *save_button_text = mx_strcmp(current_language, "UA") == 0 ? "Зберегти" : "Save";
    const char *cancel_button_text = mx_strcmp(current_language, "UA") == 0 ? "Скасувати" : "Cancel";

    GtkWidget *edit_dialog = gtk_dialog_new_with_buttons(
        dialog_title,
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        save_button_text, GTK_RESPONSE_OK,
        cancel_button_text, GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(edit_dialog));

    GtkWidget *entry_edit = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_edit),
        mx_strcmp(current_language, "UA") == 0 ? "Введіть нове повідомлення" : "Enter new message");

    gtk_box_pack_start(GTK_BOX(content_area), entry_edit, TRUE, TRUE, 5);
    gtk_widget_show_all(edit_dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(edit_dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *new_message = gtk_entry_get_text(GTK_ENTRY(entry_edit));

        if (mx_strlen(new_message) > 0) {
            char server_response[256];
            if (send_to_server("EDIT_MESSAGE", g_strdup_printf("%d|\"%s\"", message_id, new_message), "", "", server_response, sizeof(server_response)) == 0) {
                if (mx_strcmp(server_response, "OK") == 0) {
                    load_chat_messages(
                        g_object_get_data(G_OBJECT(parent_window), "chat_container"),
                        g_object_get_data(G_OBJECT(parent_window), "current_user"),
                        g_object_get_data(G_OBJECT(parent_window), "selected_user")
                    );
                } else {
                    fprintf(stderr, "Помилка на сервері: %s\n", server_response);
                }
            } else {
                fprintf(stderr, "Не вдалося надіслати запит на сервер для оновлення повідомлення.\n");
            }
        }
    }

    gtk_widget_destroy(edit_dialog);
}
