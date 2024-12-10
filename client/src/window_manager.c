#include "../inc/client.h"

static GtkWidget *main_window = NULL;
static GtkWidget *chat_window = NULL;
static GtkWidget *sticker_window = NULL;

void set_main_window(GtkWidget *window) {
    main_window = window;
}

GtkWidget *get_main_window() {
    return main_window;
}

void set_chat_window(GtkWidget *window) {
    chat_window = window;
}

GtkWidget *get_chat_window() {
    return chat_window;
}

void set_sticker_window(GtkWidget *window) {
    sticker_window = window;
}

GtkWidget *get_sticker_window() {
    return sticker_window;
}
