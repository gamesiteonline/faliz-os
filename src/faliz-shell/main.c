#include <adwaita.h>
#include <gtk/gtk.h>
#include <time.h>

#define APP_ID "com.faliz.OS.Shell"

typedef struct {
    GtkWidget *clock;
    GtkWidget *launcher;
} FalizShell;

static void launch_uri(const char *uri) {
    GError *error = NULL;
    if (!g_app_info_launch_default_for_uri(uri, NULL, &error)) {
        g_warning("Could not launch %s: %s", uri, error ? error->message : "unknown error");
        g_clear_error(&error);
    }
}

static void launch_terminal(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    GError *error = NULL;
    const char *terminal = g_getenv("TERMINAL");
    if (!terminal) terminal = "kgx";
    if (!g_spawn_command_line_async(terminal, &error)) {
        g_warning("Could not launch terminal: %s", error ? error->message : "unknown error");
        g_clear_error(&error);
    }
}

static void launch_files(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    launch_uri("file:///home");
}

static void launch_settings(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    GError *error = NULL;
    if (!g_spawn_command_line_async("gnome-control-center", &error)) {
        g_warning("Could not launch system settings: %s", error ? error->message : "not installed");
        g_clear_error(&error);
    }
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

static GtkWidget *pill_button(const char *label, const char *icon, GCallback callback, gpointer data) {
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *image = gtk_image_new_from_icon_name(icon);
    GtkWidget *text = gtk_label_new(label);
    gtk_widget_add_css_class(button, "pill-button");
    gtk_box_append(GTK_BOX(box), image);
    gtk_box_append(GTK_BOX(box), text);
    gtk_button_set_child(GTK_BUTTON(button), box);
    if (callback) g_signal_connect(button, "clicked", callback, data);
    return button;
}

static void activate(GApplication *app, gpointer user_data) {
    (void)user_data;
    FalizShell shell = {0};
    GtkCssProvider *css = gtk_css_provider_new();
    const char *css_path = g_file_test("/usr/share/faliz/faliz.css", G_FILE_TEST_EXISTS)
        ? "/usr/share/faliz/faliz.css" : "faliz.css";
    gtk_css_provider_load_from_path(css, css_path);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);
    GtkWidget *window = adw_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "FALIZ OS");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 800);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(root, "faliz-desktop");
    gtk_window_set_child(GTK_WINDOW(window), root);

    GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_add_css_class(top, "topbar");
    gtk_box_append(GTK_BOX(root), top);
    GtkWidget *brand = gtk_label_new("FALIZ");
    gtk_widget_add_css_class(brand, "brand");
    gtk_box_append(GTK_BOX(top), brand);
    GtkWidget *status = gtk_label_new("Workspace 1   •   Wayland session");
    gtk_widget_add_css_class(status, "muted");
    gtk_widget_set_hexpand(status, TRUE);
    gtk_widget_set_halign(status, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(top), status);
    shell.clock = gtk_label_new("");
    gtk_widget_add_css_class(shell.clock, "clock");
    gtk_box_append(GTK_BOX(top), shell.clock);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_widget_set_valign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(content, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(root), content);
    GtkWidget *headline = gtk_label_new("Experience computing differently.");
    gtk_widget_add_css_class(headline, "headline");
    gtk_box_append(GTK_BOX(content), headline);
    GtkWidget *sub = gtk_label_new("A calm, capable desktop for focused work.");
    gtk_widget_add_css_class(sub, "subtitle");
    gtk_box_append(GTK_BOX(content), sub);

    GtkWidget *dock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_add_css_class(dock, "dock");
    gtk_widget_set_halign(dock, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(root), dock);
    shell.launcher = pill_button("Applications", "view-grid-symbolic", NULL, NULL);
    gtk_box_append(GTK_BOX(dock), shell.launcher);
    gtk_box_append(GTK_BOX(dock), pill_button("Files", "folder-symbolic", G_CALLBACK(launch_files), NULL));
    gtk_box_append(GTK_BOX(dock), pill_button("Terminal", "utilities-terminal-symbolic", G_CALLBACK(launch_terminal), NULL));
    gtk_box_append(GTK_BOX(dock), pill_button("Settings", "preferences-system-symbolic", G_CALLBACK(launch_settings), NULL));

    update_clock(&shell);
    g_timeout_add_seconds(30, update_clock, &shell);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    if (argc > 1 && g_strcmp0(argv[1], "--version") == 0) {
        g_print("FALIZ OS shell foundation 0.1.0\n");
        return 0;
    }
    AdwApplication *app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
