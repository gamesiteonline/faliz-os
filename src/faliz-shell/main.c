#include <adwaita.h>
#include <gtk/gtk.h>

#define APP_ID "com.faliz.OS.Shell"

typedef struct {
    GtkWidget *root;
    GtkWidget *clock;
    GtkWidget *workspace_status;
    AdwToastOverlay *toasts;
    guint active_workspace;
} FalizShell;

static void show_toast(FalizShell *shell, const char *message) {
    if (shell && shell->toasts)
        adw_toast_overlay_add_toast(shell->toasts, adw_toast_new(message));
}

static void launch_command(FalizShell *shell, const char *command, const char *label) {
    GError *error = NULL;
    if (!g_spawn_command_line_async(command, &error)) {
        char *message = g_strdup_printf("Could not launch %s", label);
        show_toast(shell, message);
        g_free(message);
        g_clear_error(&error);
        return;
    }
    char *message = g_strdup_printf("Opened %s", label);
    show_toast(shell, message);
    g_free(message);
}

static void launch_uri(FalizShell *shell, const char *uri, const char *label) {
    GError *error = NULL;
    if (!g_app_info_launch_default_for_uri(uri, NULL, &error)) {
        char *message = g_strdup_printf("Could not open %s", label);
        show_toast(shell, message);
        g_free(message);
        g_clear_error(&error);
        return;
    }
    char *message = g_strdup_printf("Opened %s", label);
    show_toast(shell, message);
    g_free(message);
}

static void launch_files(GtkButton *button, gpointer user_data) {
    (void)button;
    launch_uri(user_data, "file:///home", "Files");
}

static void launch_terminal(GtkButton *button, gpointer user_data) {
    (void)button;
    const char *terminal = g_getenv("TERMINAL");
    launch_command(user_data, terminal && *terminal ? terminal : "kgx", "Terminal");
}

static void launch_settings(GtkButton *button, gpointer user_data) {
    (void)button;
    launch_command(user_data, "faliz-settings", "Settings");
}

static void launch_browser(GtkButton *button, gpointer user_data) {
    (void)button;
    launch_uri(user_data, "https://www.example.com", "Browser");
}

static gboolean update_clock(gpointer user_data) {
    FalizShell *shell = user_data;
    GDateTime *now = g_date_time_new_now_local();
    char *text = g_date_time_format(now, "%a  %H:%M");
    gtk_label_set_text(GTK_LABEL(shell->clock), text);
    g_free(text);
    g_date_time_unref(now);
    return G_SOURCE_CONTINUE;
}

static void select_workspace(GtkButton *button, gpointer user_data) {
    FalizShell *shell = user_data;
    const char *workspace = gtk_button_get_label(button);
    shell->active_workspace = (guint)g_ascii_strtoull(workspace, NULL, 10);
    char *status = g_strdup_printf("Workspace %u   •   Wayland session", shell->active_workspace);
    gtk_label_set_text(GTK_LABEL(shell->workspace_status), status);
    show_toast(shell, "Workspace selected");
    g_free(status);
}

static void toggle_theme(GtkSwitch *toggle, GParamSpec *pspec, gpointer user_data) {
    (void)pspec;
    FalizShell *shell = user_data;
    gboolean light = gtk_switch_get_active(toggle);
    if (light)
        gtk_widget_add_css_class(shell->root, "light-theme");
    else
        gtk_widget_remove_css_class(shell->root, "light-theme");
    show_toast(shell, light ? "Light theme enabled" : "Dark theme enabled");
}

static GtkWidget *app_button(const char *label, const char *icon, GCallback callback, gpointer data) {
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *image = gtk_image_new_from_icon_name(icon);
    GtkWidget *text = gtk_label_new(label);
    gtk_widget_add_css_class(button, "launcher-item");
    gtk_widget_set_halign(text, GTK_ALIGN_START);
    gtk_widget_set_hexpand(text, TRUE);
    gtk_box_append(GTK_BOX(box), image);
    gtk_box_append(GTK_BOX(box), text);
    gtk_button_set_child(GTK_BUTTON(button), box);
    g_signal_connect(button, "clicked", callback, data);
    return button;
}

static GtkWidget *dock_button(const char *label, const char *icon, GCallback callback, gpointer data) {
    GtkWidget *button = gtk_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(button), icon);
    gtk_widget_set_tooltip_text(button, label);
    gtk_widget_add_css_class(button, "dock-button");
    if (callback)
        g_signal_connect(button, "clicked", callback, data);
    return button;
}

static GtkWidget *build_launcher(FalizShell *shell) {
    GtkWidget *popover = gtk_popover_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new("FALIZ Applications");
    gtk_widget_add_css_class(heading, "launcher-heading");
    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), app_button("Files", "folder-symbolic", G_CALLBACK(launch_files), shell));
    gtk_box_append(GTK_BOX(box), app_button("Terminal", "utilities-terminal-symbolic", G_CALLBACK(launch_terminal), shell));
    gtk_box_append(GTK_BOX(box), app_button("Browser", "web-browser-symbolic", G_CALLBACK(launch_browser), shell));
    gtk_box_append(GTK_BOX(box), app_button("Settings", "preferences-system-symbolic", G_CALLBACK(launch_settings), shell));
    gtk_widget_add_css_class(box, "launcher-popover");
    gtk_popover_set_child(GTK_POPOVER(popover), box);
    return popover;
}

static void activate(GApplication *app, gpointer user_data) {
    (void)user_data;
    static FalizShell shell = {0};
    shell.active_workspace = 1;

    GtkCssProvider *css = gtk_css_provider_new();
    const char *css_path = g_file_test("/usr/share/faliz/faliz.css", G_FILE_TEST_EXISTS)
        ? "/usr/share/faliz/faliz.css"
        : (g_file_test("src/faliz-shell/faliz.css", G_FILE_TEST_EXISTS)
            ? "src/faliz-shell/faliz.css" : "faliz.css");
    gtk_css_provider_load_from_path(css, css_path);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);

    GtkWidget *window = adw_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "FALIZ OS");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 800);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    shell.toasts = ADW_TOAST_OVERLAY(adw_toast_overlay_new());
    shell.root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(shell.root, "faliz-desktop");
    adw_toast_overlay_set_child(shell.toasts, shell.root);
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), GTK_WIDGET(shell.toasts));

    GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_add_css_class(top, "topbar");
    gtk_box_append(GTK_BOX(shell.root), top);

    GtkWidget *brand = gtk_label_new("FALIZ");
    gtk_widget_add_css_class(brand, "brand");
    gtk_box_append(GTK_BOX(top), brand);

    shell.workspace_status = gtk_label_new("Workspace 1   •   Wayland session");
    gtk_widget_add_css_class(shell.workspace_status, "muted");
    gtk_widget_set_hexpand(shell.workspace_status, TRUE);
    gtk_widget_set_halign(shell.workspace_status, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(top), shell.workspace_status);

    shell.clock = gtk_label_new("");
    gtk_widget_add_css_class(shell.clock, "clock");
    gtk_box_append(GTK_BOX(top), shell.clock);

    GtkWidget *workspace_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(workspace_bar, "workspace-bar");
    gtk_widget_set_halign(workspace_bar, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(shell.root), workspace_bar);
    for (int i = 1; i <= 4; i++) {
        char label[2] = {(char)('0' + i), '\0'};
        GtkWidget *button = gtk_button_new_with_label(label);
        gtk_widget_add_css_class(button, "workspace-button");
        g_signal_connect(button, "clicked", G_CALLBACK(select_workspace), &shell);
        gtk_box_append(GTK_BOX(workspace_bar), button);
    }

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_widget_set_valign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(content, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(shell.root), content);

    GtkWidget *headline = gtk_label_new("Experience computing differently.");
    gtk_widget_add_css_class(headline, "headline");
    gtk_box_append(GTK_BOX(content), headline);
    GtkWidget *sub = gtk_label_new("A calm, capable desktop for focused work.");
    gtk_widget_add_css_class(sub, "subtitle");
    gtk_box_append(GTK_BOX(content), sub);

    GtkWidget *hint = gtk_label_new("FALIZ BUILD 02  •  sway Wayland session  •  4 workspaces");
    gtk_widget_add_css_class(hint, "session-hint");
    gtk_box_append(GTK_BOX(content), hint);

    GtkWidget *dock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_add_css_class(dock, "dock");
    gtk_widget_set_halign(dock, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(shell.root), dock);

    GtkWidget *launcher_button = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(launcher_button), "Applications");
    gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(launcher_button), "view-grid-symbolic");
    gtk_widget_add_css_class(launcher_button, "launcher-button");
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(launcher_button), build_launcher(&shell));
    gtk_box_append(GTK_BOX(dock), launcher_button);
    gtk_box_append(GTK_BOX(dock), dock_button("Files", "folder-symbolic", G_CALLBACK(launch_files), &shell));
    gtk_box_append(GTK_BOX(dock), dock_button("Terminal", "utilities-terminal-symbolic", G_CALLBACK(launch_terminal), &shell));
    gtk_box_append(GTK_BOX(dock), dock_button("Browser", "web-browser-symbolic", G_CALLBACK(launch_browser), &shell));
    gtk_box_append(GTK_BOX(dock), dock_button("Settings", "preferences-system-symbolic", G_CALLBACK(launch_settings), &shell));

    GtkWidget *theme = gtk_switch_new();
    gtk_widget_set_tooltip_text(theme, "Toggle light theme");
    gtk_widget_add_css_class(theme, "theme-switch");
    g_signal_connect(theme, "notify::active", G_CALLBACK(toggle_theme), &shell);
    gtk_box_append(GTK_BOX(dock), theme);

    update_clock(&shell);
    g_timeout_add_seconds(30, update_clock, &shell);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    if (argc > 1 && g_strcmp0(argv[1], "--version") == 0) {
        g_print("FALIZ OS shell BUILD 02 0.2.0\n");
        return 0;
    }
    AdwApplication *app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
