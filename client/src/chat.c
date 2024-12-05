/// chat.c
#include "../inc/client.h"
#include "style_chat.c"
// Переменная для хранения буфера текстового виджета
GtkTextBuffer *chat_buffer = NULL;

extern char current_language[3];

// Объявление глобальной переменной
GtkWidget *main_vertical_box;


char current_language[3] = "UA"; // Start with Ukrainian

void on_logout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);
    gtk_widget_hide(window);  // Скрываем текущее окно чата
    create_login_window();  // Создаем и показываем окно логина
}

void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    exit(0);  // Завершает программу
}

void create_chat_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Чат");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);



     g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Основной вертикальный контейнер
    GtkWidget *main_vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(main_vertical_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_vertical_box);

    // Верхняя панель с именем пользователя
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(header_box), TRUE);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), header_box, FALSE, FALSE, 0);

    char user_label_text[128];
    snprintf(user_label_text, sizeof(user_label_text), "Користувач: %s", current_user);
    GtkWidget *user_label = gtk_label_new(user_label_text);

    // Устанавливаем выравнивание метки пользователя влево
    gtk_widget_set_halign(user_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(header_box), user_label, TRUE, TRUE, 15);


    // Панель для кнопок (Выйти и Настройки)
    GtkWidget *logout_and_settings_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(header_box), logout_and_settings_box, FALSE, FALSE, 0);  // Отступ между кнопками

    gtk_box_set_homogeneous(GTK_BOX(header_box), FALSE); // Чтобы элементы не растягивались
	gtk_box_set_homogeneous(GTK_BOX(logout_and_settings_box), FALSE); // Для кнопок


    // Кнопка настроек
    GtkWidget *settings_button = gtk_menu_button_new();
    GtkWidget *settings_icon = gtk_image_new_from_icon_name("emblem-system", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(settings_button), settings_icon);
    gtk_box_pack_start(GTK_BOX(logout_and_settings_box), settings_button, FALSE, FALSE, 0); // Убираем большой отступ здесь

    // Добавим отступы слева, чтобы кнопки переместились вправо
    gtk_widget_set_margin_start(logout_and_settings_box, 350); // Сдвигаем блок с кнопками вправо на 105 пикселей

    // Создаем выпадающее меню для кнопки настроек
    GtkWidget *settings_menu = gtk_menu_new();

    GtkWidget *theme_item = gtk_menu_item_new_with_label("Змінити тему");
    g_signal_connect(theme_item, "activate", G_CALLBACK(on_change_theme), window); // Функция для смены темы
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), theme_item);

    GtkWidget *about_item = gtk_menu_item_new_with_label("Про програму");
    g_signal_connect(about_item, "activate", G_CALLBACK(on_about_clicked), window); // Функция для показа информации о программе
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), about_item);

    GtkWidget *language_item = gtk_menu_item_new_with_label("Змінити мову: UA");
    g_signal_connect(language_item, "activate", G_CALLBACK(on_switch_language_clicked), window); // Функция для смены языка
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), language_item);

    GtkWidget *logout_item = gtk_menu_item_new_with_label("Вийти");
    g_signal_connect(logout_item, "activate", G_CALLBACK(on_logout_clicked), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), logout_item);

    // Устанавливаем меню для кнопки настроек
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(settings_button), settings_menu);
    gtk_widget_show_all(settings_menu);

    // Панель для кнопки шестеренки
    GtkWidget *gear_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), gear_button_box, FALSE, FALSE, 0);

    // Основной горизонтальный контейнер
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vertical_box), main_box, TRUE, TRUE, 0);

	// Список пользователей (левая панель)
	GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_vexpand(left_panel, TRUE);
	gtk_box_pack_start(GTK_BOX(main_box), left_panel, FALSE, TRUE, 0);

    GtkWidget *top_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); // Убираем лишний spacing
    gtk_box_pack_start(GTK_BOX(left_panel), top_container, FALSE, FALSE, 0);
    gtk_widget_set_margin_top(top_container, 0); // Убираем отступ сверху
    gtk_widget_set_margin_bottom(left_panel, 0); // Убираем отступ снизу



	// Объявляем и создаем список пользователей перед вызовом add_search_bar
	GtkWidget *users_list = gtk_list_box_new();
	gtk_widget_set_vexpand(users_list, TRUE);

	// Теперь можно передать users_list в add_search_bar
	add_search_bar(top_container, users_list);

	// Добавляем users_list в прокручиваемую область
	GtkWidget *scrolled_users = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_users), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled_users, 150, -1);

	gtk_container_add(GTK_CONTAINER(scrolled_users), users_list);
	gtk_box_pack_start(GTK_BOX(left_panel), scrolled_users, TRUE, TRUE, 0);

    g_signal_connect(users_list, "row-selected", G_CALLBACK(on_user_selected), window);

    // Правая часть с чатом
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), right_box, TRUE, TRUE, 0);

    GtkWidget *chat_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_vexpand(chat_container, TRUE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), chat_container);
    gtk_box_pack_start(GTK_BOX(right_box), scrolled_window, TRUE, TRUE, 0);

    // Сохраняем chat_container и scrolled_window для дальнейшего использования
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

    // Кнопка для выбора стикеров
    GtkWidget *sticker_button = gtk_button_new_with_label("🙂");
    gtk_box_pack_start(GTK_BOX(message_box), sticker_button, FALSE, FALSE, 0);

    // Связь объектов с данными окна для обновления интерфейса
    g_object_set_data(G_OBJECT(window), "send_button", button_send);
    g_object_set_data(G_OBJECT(window), "sticker_button", sticker_button);

	        // Добавляем в g_object данные для доступа в update_text_labels
    g_object_set_data(G_OBJECT(window), "window", window);
    g_object_set_data(G_OBJECT(window), "user_label", user_label);
    g_object_set_data(G_OBJECT(window), "theme_item", theme_item);
    g_object_set_data(G_OBJECT(window), "language_item", language_item);
    g_object_set_data(G_OBJECT(window), "logout_item", logout_item);


    // Создаем GtkPopover для стикеров
    GtkWidget *sticker_popover = gtk_popover_new(sticker_button);
    GtkWidget *sticker_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sticker_scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(sticker_scrolled_window, 200, 200); // Размер области прокрутки
    gtk_container_add(GTK_CONTAINER(sticker_popover), sticker_scrolled_window);

    GtkWidget *sticker_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(sticker_scrolled_window), sticker_grid);

    int num_stickers = num_stickers;

    for (int i = 0; i < num_stickers; i++) {
        GtkWidget *sticker_item_button = gtk_button_new_with_label(stickers[i]);
        g_signal_connect(sticker_item_button, "clicked", G_CALLBACK(on_sticker_selected), entry_message);
        gtk_grid_attach(GTK_GRID(sticker_grid), sticker_item_button, i % 3, i / 3, 1, 1); // Располагаем кнопки в сетке
    }

    // Подключение кнопки к показу поповера
    g_signal_connect(sticker_button, "clicked", G_CALLBACK(on_sticker_button_clicked), sticker_popover);

    // Добавление CSS стилей
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
void add_message_to_chat(GtkWidget *chat_container, const char *sender, const char *time, const char *message, int message_id) {
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(message_box, 5);
    gtk_widget_set_margin_bottom(message_box, 5);
    gtk_widget_set_margin_start(message_box, 10);
    gtk_widget_set_margin_end(message_box, 10);

    // Создаём метку с информацией о пользователе и времени
    char sender_and_time[128];
    snprintf(sender_and_time, sizeof(sender_and_time), "%s [%s]", sender, time);
    GtkWidget *meta_label = gtk_label_new(sender_and_time);
    gtk_widget_set_halign(meta_label, GTK_ALIGN_START);

    // Создаём метку с текстом сообщения
    GtkWidget *message_label = gtk_label_new(message);
    gtk_label_set_line_wrap(GTK_LABEL(message_label), TRUE);
    gtk_widget_set_halign(message_label, GTK_ALIGN_START);

    // Контейнер для кнопок
    GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(buttons_box, GTK_ALIGN_END);

    // Кнопка "Редактировать"
    GtkWidget *edit_button = gtk_button_new_with_label("✏️");
    gtk_widget_set_name(edit_button, "edit-button");
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_message_clicked), GINT_TO_POINTER(message_id));

    // Кнопка "Удалить"
    GtkWidget *delete_button = gtk_button_new_with_label("❌");
    gtk_widget_set_name(delete_button, "delete-button");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_message_clicked), GINT_TO_POINTER(message_id));

    // Добавляем кнопки в контейнер кнопок
    gtk_box_pack_start(GTK_BOX(buttons_box), edit_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttons_box), delete_button, FALSE, FALSE, 0);

    // Контейнер для сообщения (слева текст, справа кнопки)
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(content_box), message_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content_box), buttons_box, FALSE, FALSE, 0);

    // Добавляем содержимое в message_box
    gtk_box_pack_start(GTK_BOX(message_box), meta_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(message_box), content_box, TRUE, TRUE, 0);

    // Добавляем message_box в chat_container
    gtk_box_pack_start(GTK_BOX(chat_container), message_box, FALSE, FALSE, 0);

    gtk_widget_show_all(chat_container);
}






static gboolean notifications_enabled = TRUE; // Уведомления включены по умолчанию

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
    const char *new_theme = (g_strcmp0(current_theme, "dark") == 0) ? "light" : "dark";  // Заменили strcmp на g_strcmp0
    set_chat_theme(provider, new_theme);
    g_object_unref(provider);
}

void on_about_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(user_data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Это пример чата на GTK+.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void update_text_labels(gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);
    gtk_window_set_title(GTK_WINDOW(window), g_strcmp0(current_language, "UA") == 0 ? "Чат" : "Chat");

    GtkWidget *search_entry = gtk_search_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Пошук контактів...");
    gtk_box_pack_start(GTK_BOX(main_vertical_box), search_entry, FALSE, FALSE, 0);


    // Retrieve widget pointers for updating labels
    GtkWidget *logout_button = g_object_get_data(G_OBJECT(window), "logout_button");
    GtkWidget *send_button = g_object_get_data(G_OBJECT(window), "send_button");
    GtkWidget *sticker_button = g_object_get_data(G_OBJECT(window), "sticker_button");
    GtkWidget *user_label = g_object_get_data(G_OBJECT(window), "user_label");
    GtkWidget *entry_message = g_object_get_data(G_OBJECT(window), "entry_message");

    // Update the menu items based on the selected language
    GtkWidget *theme_item = g_object_get_data(G_OBJECT(window), "theme_item");
    GtkWidget *about_item = g_object_get_data(G_OBJECT(window), "about_item");
    GtkWidget *language_item = g_object_get_data(G_OBJECT(window), "language_item");
    GtkWidget *notifications_item = g_object_get_data(G_OBJECT(window), "notifications_item");
    GtkWidget *logout_item = g_object_get_data(G_OBJECT(window), "logout_item");

    if (g_strcmp0(current_language, "UA") == 0) {
        gtk_menu_item_set_label(GTK_MENU_ITEM(theme_item), "Змінити тему");
        gtk_menu_item_set_label(GTK_MENU_ITEM(about_item), "Про програму");
        gtk_menu_item_set_label(GTK_MENU_ITEM(language_item), "Змінити мову: UA");
        gtk_menu_item_set_label(GTK_MENU_ITEM(logout_item), "Вийти");
    } else { // English
        gtk_menu_item_set_label(GTK_MENU_ITEM(theme_item), "Change Theme");
        gtk_menu_item_set_label(GTK_MENU_ITEM(about_item), "About");
        gtk_menu_item_set_label(GTK_MENU_ITEM(language_item), "Switch Language: ENG");
        gtk_menu_item_set_label(GTK_MENU_ITEM(logout_item), "Logout");
    }

    // Обновление строки поиска
    GtkWidget *search_bar = g_object_get_data(G_OBJECT(window), "search_entry");
    if (search_bar) {
        gtk_entry_set_placeholder_text(GTK_ENTRY(search_bar),
            g_strcmp0(current_language, "UA") == 0 ? "Пошук контактів..." : "Search contacts...");
    }



    // Update button labels and placeholder texts
    if (g_strcmp0(current_language, "UA") == 0) {
        // Buttons
        gtk_button_set_label(GTK_BUTTON(logout_button), "Вийти");
        gtk_button_set_label(GTK_BUTTON(send_button), "Надіслати");
        gtk_button_set_label(GTK_BUTTON(sticker_button), "🙂");

        // Update user label
        if (user_label) {
            char user_label_text[128];
            snprintf(user_label_text, sizeof(user_label_text), "Користувач: %s", current_user);
            gtk_label_set_text(GTK_LABEL(user_label), user_label_text);
        }

        // Update placeholder text
        if (entry_message) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Введіть повідомлення");
        }
    } else { // English
        // Buttons
        gtk_button_set_label(GTK_BUTTON(logout_button), "Logout");
        gtk_button_set_label(GTK_BUTTON(send_button), "Send");
        gtk_button_set_label(GTK_BUTTON(sticker_button), "🙂");

        // Update user label
        if (user_label) {
            char user_label_text[128];
            snprintf(user_label_text, sizeof(user_label_text), "User: %s", current_user);
            gtk_label_set_text(GTK_LABEL(user_label), user_label_text);
        }

        // Update placeholder text
        if (entry_message) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(entry_message), "Enter message");
        }
    }
}


void on_delete_message_clicked(GtkButton *button, gpointer user_data) {
    int message_id = GPOINTER_TO_INT(user_data);
    printf("Request to delete message with ID: %d\n", message_id);

    char response[256];

    if (send_to_server("DELETE_MESSAGE", g_strdup_printf("%d", message_id), "", "", response, sizeof(response)) == 0) {
        printf("Server response: %s\n", response);

        if (strcmp(response, "OK") == 0) {
            gtk_widget_destroy(gtk_widget_get_parent(GTK_WIDGET(button))); // Удаляем виджет сообщения
        } else {
            fprintf(stderr, "Failed to delete message on server. Response: %s\n", response);
        }
    } else {
        fprintf(stderr, "Failed to send DELETE_MESSAGE command to server.\n");
    }
}

void on_switch_language_clicked(GtkMenuItem *menuitem, gpointer user_data) {
    if (g_strcmp0(current_language, "UA") == 0) {
        current_language[0] = 'E';  // Change to English
        current_language[1] = 'N';
    } else {
        current_language[0] = 'U';  // Change to Ukrainian
        current_language[1] = 'A';
    }
    update_text_labels(user_data);  // Update UI elements
}

void on_search_entry_changed(GtkSearchEntry *entry, gpointer user_data) {
    GtkWidget *users_list = GTK_WIDGET(user_data);
    const char *search_text = gtk_entry_get_text(GTK_ENTRY(entry));

    // Пройдёмся по всем строкам в списке
    GList *children = gtk_container_get_children(GTK_CONTAINER(users_list));
    for (GList *l = children; l != NULL; l = l->next) {
        GtkWidget *row = GTK_WIDGET(l->data);
        const char *label_text = gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(row))));

        // Показываем строку, если она содержит текст поиска, игнорируя регистр
        if (g_strrstr(g_ascii_strdown(label_text, -1), g_ascii_strdown(search_text, -1)) != NULL) {
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

    // Сохраняем search_bar в g_object
    GtkWidget *window = gtk_widget_get_toplevel(main_vertical_box);
    g_object_set_data(G_OBJECT(window), "search_entry", search_bar);

    g_signal_connect(search_bar, "search-changed", G_CALLBACK(on_search_entry_changed), users_list);
}



void on_edit_message_clicked(GtkButton *button, gpointer user_data) {
    int message_id = GPOINTER_TO_INT(user_data);

    // Получаем родительское окно для диалога
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Создаём окно редактирования
    const char *dialog_title = g_strcmp0(current_language, "UA") == 0 ? "Редагувати повідомлення" : "Edit message";
    const char *save_button_text = g_strcmp0(current_language, "UA") == 0 ? "Зберегти" : "Save";
    const char *cancel_button_text = g_strcmp0(current_language, "UA") == 0 ? "Скасувати" : "Cancel";

    GtkWidget *edit_dialog = gtk_dialog_new_with_buttons(
        dialog_title,
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        save_button_text, GTK_RESPONSE_OK,
        cancel_button_text, GTK_RESPONSE_CANCEL,
        NULL
    );

    // Получаем область содержимого диалога
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(edit_dialog));

    // Создаём поле ввода
    GtkWidget *entry_edit = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_edit),
        g_strcmp0(current_language, "UA") == 0 ? "Введіть нове повідомлення" : "Enter new message");

    gtk_box_pack_start(GTK_BOX(content_area), entry_edit, TRUE, TRUE, 5);
    gtk_widget_show_all(edit_dialog);

    // Обработка ответа
    gint response = gtk_dialog_run(GTK_DIALOG(edit_dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *new_message = gtk_entry_get_text(GTK_ENTRY(entry_edit));

        if (strlen(new_message) > 0) {
            // Отправляем запрос на сервер для обновления сообщения
            char server_response[256];  // Переименована переменная для избежания конфликта
            if (send_to_server("EDIT_MESSAGE", g_strdup_printf("%d|\"%s\"", message_id, new_message), "", "", server_response, sizeof(server_response)) == 0) {
                if (strcmp(server_response, "OK") == 0) {
                    printf("Сообщение успешно обновлено\n");
                    // Обновление UI
                    load_chat_messages(
                        g_object_get_data(G_OBJECT(parent_window), "chat_container"),
                        g_object_get_data(G_OBJECT(parent_window), "current_user"),
                        g_object_get_data(G_OBJECT(parent_window), "selected_user")
                    );
                } else {
                    fprintf(stderr, "Ошибка на сервере: %s\n", server_response);
                }
            } else {
                fprintf(stderr, "Не удалось отправить запрос на сервер для обновления сообщения.\n");
            }
        }
    }

    // Уничтожаем диалоговое окно
    gtk_widget_destroy(edit_dialog);
}



