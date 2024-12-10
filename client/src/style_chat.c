
void set_chat_theme(GtkCssProvider *provider, const char *theme) {
    const char *light_css =
    "window { background-color: #ECEFF4; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }"
    "label { font-size: 16px; color: #2E3440; font-weight: bold; }"
    "entry { border: 2px solid #4C566A; border-radius: 5px; padding: 10px; margin-bottom: 15px; background-color: #D8DEE9; color: #2E3440; font-size: 14px; }"
    "entry:focus { border-color: #3B4252; }"
    "button { background-color: #5E81AC; color: white; border-radius: 5px; padding: 10px 20px; font-weight: bold; transition: background-color 0.3s ease; font-size: 14px; }"
    "button:hover { background-color: #81A1C1; }"
    "button#edit-button { font-size: 12px; padding: 3px 6px; color: #FFA500; background-color: transparent; }"
    "button#edit-button:hover { color: #FF8C00; }"
    "button#delete-button { font-size: 12px; padding: 3px 6px; color: red; background-color: transparent; }"
    "button#delete-button:hover { color: darkred; }"
    "scrolledwindow { background-color: #D8DEE9; border-radius: 5px; }"
    "listbox { background-color: #D8DEE9; color: #2E3440; border-radius: 5px; padding: 10px; margin-top: 10px; }"
    "listbox row:hover { background-color: #B2B9C0; }";

const char *dark_css =
    "window { background-color: #2E3440; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }"
    "label { font-size: 16px; color: #D8DEE9; font-weight: bold; }"
    "entry { border: 2px solid #4C566A; border-radius: 5px; padding: 10px; margin-bottom: 15px; background-color: #434C5E; color: #D8DEE9; font-size: 14px; }"
    "entry:focus { border-color: #81A1C1; }"
    "button { background-color: #5E81AC; color: white; border-radius: 5px; padding: 10px 20px; font-weight: bold; transition: background-color 0.3s ease; font-size: 14px; }"
    "button:hover { background-color: #81A1C1; }"
    "button#edit-button { font-size: 12px; padding: 3px 6px; color: #FFA500; background-color: transparent; }"
    "button#edit-button:hover { color: #FF8C00; }"
    "button#delete-button { font-size: 12px; padding: 3px 6px; color: red; background-color: transparent; }"
    "button#delete-button:hover { color: darkred; }"
    "scrolledwindow { background-color: #434C5E; border-radius: 5px; }"
    "listbox { background-color: #434C5E; color: #D8DEE9; border-radius: 5px; padding: 10px; margin-top: 10px; }"
    "listbox row:hover { background-color: #3B4252; }";



    if (mx_strcmp(theme, "dark") == 0) {
        gtk_css_provider_load_from_data(provider, dark_css, -1, NULL);
    } else {
        gtk_css_provider_load_from_data(provider, light_css, -1, NULL);
    }
}
