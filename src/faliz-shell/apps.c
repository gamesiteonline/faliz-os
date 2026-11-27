#include <adwaita.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

#define APP_ID "com.faliz.OS.Apps"
#define VERSION "FALIZ native apps BUILD 04.1 0.4.1"

typedef enum {
    APP_FILES,
    APP_STORE,
    APP_NOTES,
    APP_CALCULATOR,
    APP_SCREENSHOT,
    APP_MONITOR,
    APP_MEDIA
} AppMode;

typedef struct {
    AppMode mode;
    GtkWidget *window;
    GtkWidget *body;
    GtkWidget *status;
    GtkWidget *file_list;
    GtkWidget *path_entry;
    GtkWidget *notes_view;
    GtkWidget *calc_entry;
    GtkWidget *calc_result;
    GtkWidget *monitor_label;
    char *current_dir;
} FalizApp;

static void status(FalizApp *app, const char *message) {
    if (app->status) gtk_label_set_text(GTK_LABEL(app->status), message);
}

static GtkWidget *heading(const char *title, const char *subtitle) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *title_label = gtk_label_new(title);
    GtkWidget *subtitle_label = gtk_label_new(subtitle);
    gtk_widget_add_css_class(title_label, "title-2");
    gtk_widget_add_css_class(subtitle_label, "dim-label");
    gtk_widget_set_halign(title_label, GTK_ALIGN_START);
    gtk_widget_set_halign(subtitle_label, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(subtitle_label), TRUE);
    gtk_box_append(GTK_BOX(box), title_label);
    gtk_box_append(GTK_BOX(box), subtitle_label);
    return box;
}

static GtkWidget *button(const char *label, GCallback callback, gpointer data) {
    GtkWidget *item = gtk_button_new_with_label(label);
    gtk_widget_set_halign(item, GTK_ALIGN_START);
    g_signal_connect(item, "clicked", callback, data);
    return item;
}

static void launch_command(FalizApp *app, const char *command, const char *success) {
    GError *error = NULL;
    if (!g_spawn_command_line_async(command, &error)) {
        status(app, error ? error->message : "Could not start the requested application.");
        g_clear_error(&error);
        return;
    }
    status(app, success);
}

static void launch_file(GtkListBox *list, GtkListBoxRow *row, gpointer user_data) {
    (void)list;
    FalizApp *app = user_data;
    const char *name = g_object_get_data(G_OBJECT(row), "faliz-name");
    if (!name) return;
    char *path = g_build_filename(app->current_dir, name, NULL);
    char *uri = g_filename_to_uri(path, NULL, NULL);
    if (uri) {
        GError *error = NULL;
        if (!g_app_info_launch_default_for_uri(uri, NULL, &error)) {
            status(app, error ? error->message : "Could not open this file.");
            g_clear_error(&error);
        } else {
            status(app, "Opened the selected item.");
        }
    }
    g_free(uri);
    g_free(path);
}

static void clear_list(GtkWidget *list) {
    GtkWidget *child = gtk_widget_get_first_child(list);
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

static void refresh_files(FalizApp *app) {
    clear_list(app->file_list);
    GDir *dir = g_dir_open(app->current_dir, 0, NULL);
    if (!dir) {
        status(app, "The selected folder could not be read.");
        return;
    }
    const char *name;
    guint count = 0;
    while ((name = g_dir_read_name(dir)) != NULL) {
        if (name[0] == '.') continue;
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(name);
        gtk_widget_set_margin_start(label, 12);
        gtk_widget_set_margin_end(label, 12);
        gtk_widget_set_margin_top(label, 10);
        gtk_widget_set_margin_bottom(label, 10);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        g_object_set_data_full(G_OBJECT(row), "faliz-name", g_strdup(name), g_free);
        gtk_list_box_append(GTK_LIST_BOX(app->file_list), row);
        count++;
    }
    g_dir_close(dir);
    char *message = g_strdup_printf("%u items in %s", count, app->current_dir);
    status(app, message);
    g_free(message);
}

static void files_path_changed(GtkEditable *editable, gpointer user_data) {
    FalizApp *app = user_data;
    const char *path = gtk_editable_get_text(editable);
    if (!g_file_test(path, G_FILE_TEST_IS_DIR)) {
        status(app, "Enter an existing folder path.");
        return;
    }
    g_free(app->current_dir);
    app->current_dir = g_strdup(path);
    refresh_files(app);
}

static void files_home(GtkButton *button, gpointer user_data) {
    (void)button;
    FalizApp *app = user_data;
    const char *home = g_get_home_dir();
    gtk_editable_set_text(GTK_EDITABLE(app->path_entry), home);
    files_path_changed(GTK_EDITABLE(app->path_entry), app);
}

static void files_create_folder(GtkButton *button, gpointer user_data) {
    (void)button;
    FalizApp *app = user_data;
    char *path = g_build_filename(app->current_dir, "New Folder", NULL);
    if (g_mkdir(path, 0755) != 0 && errno != EEXIST) {
        status(app, "Could not create the folder.");
    } else {
        refresh_files(app);
        status(app, "Created New Folder.");
    }
    g_free(path);
}

static GtkWidget *build_files(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_box_append(GTK_BOX(box), heading("FALIZ Files", "Browse your home folder, open items with the default app, and create folders safely."));
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    app->path_entry = gtk_entry_new();
    gtk_widget_set_hexpand(app->path_entry, TRUE);
    gtk_editable_set_text(GTK_EDITABLE(app->path_entry), g_get_home_dir());
    g_signal_connect(app->path_entry, "activate", G_CALLBACK(files_path_changed), app);
    gtk_box_append(GTK_BOX(toolbar), app->path_entry);
    gtk_box_append(GTK_BOX(toolbar), button("Home", G_CALLBACK(files_home), app));
    gtk_box_append(GTK_BOX(toolbar), button("New folder", G_CALLBACK(files_create_folder), app));
    gtk_box_append(GTK_BOX(box), toolbar);
    app->file_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(app->file_list), GTK_SELECTION_SINGLE);
    g_signal_connect(app->file_list, "row-activated", G_CALLBACK(launch_file), app);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), app->file_list);
    gtk_box_append(GTK_BOX(box), scroll);
    app->current_dir = g_strdup(g_get_home_dir());
    refresh_files(app);
    return box;
}

static void save_notes(GtkButton *button, gpointer user_data) {
    (void)button;
    FalizApp *app = user_data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->notes_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    const char *documents = g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS);
    if (!documents) documents = g_get_home_dir();
    char *dir = g_build_filename(documents, "FALIZ Notes", NULL);
    if (!g_file_test(dir, G_FILE_TEST_IS_DIR)) g_mkdir_with_parents(dir, 0755);
    char *path = g_build_filename(dir, "notes.txt", NULL);
    GError *error = NULL;
    if (g_file_set_contents(path, text, -1, &error)) status(app, "Notes saved to Documents/FALIZ Notes/notes.txt");
    else { status(app, error ? error->message : "Could not save notes."); g_clear_error(&error); }
    g_free(text); g_free(path); g_free(dir);
}

static void load_notes(FalizApp *app) {
    const char *documents = g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS);
    if (!documents) documents = g_get_home_dir();
    char *path = g_build_filename(documents, "FALIZ Notes", "notes.txt", NULL);
    char *text = NULL;
    if (g_file_get_contents(path, &text, NULL, NULL)) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->notes_view)), text, -1);
        status(app, "Loaded your saved FALIZ note.");
    } else status(app, "Start writing. Save creates a local FALIZ Notes file.");
    g_free(text); g_free(path);
}

static GtkWidget *build_notes(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_box_append(GTK_BOX(box), heading("FALIZ Notes", "A distraction-free local note editor. Your note is saved in your Documents folder."));
    app->notes_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->notes_view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_vexpand(app->notes_view, TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), app->notes_view);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_box_append(GTK_BOX(box), button("Save note", G_CALLBACK(save_notes), app));
    load_notes(app);
    return box;
}

static gboolean calculate(const char *input, double *result) {
    char *end = NULL;
    double left = g_ascii_strtod(input, &end);
    if (end == input) return FALSE;
    while (g_ascii_isspace(*end)) end++;
    char op = *end++;
    double right = g_ascii_strtod(end, &end);
    if (end == NULL) return FALSE;
    switch (op) {
        case '+': *result = left + right; break;
        case '-': *result = left - right; break;
        case '*': *result = left * right; break;
        case '/': if (right == 0) return FALSE; *result = left / right; break;
        default: return FALSE;
    }
    return isfinite(*result);
}

static void calculate_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    FalizApp *app = user_data;
    double result;
    if (calculate(gtk_editable_get_text(GTK_EDITABLE(app->calc_entry)), &result)) {
        char *text = g_strdup_printf("= %.10g", result);
        gtk_label_set_text(GTK_LABEL(app->calc_result), text);
        status(app, "Calculation complete.");
        g_free(text);
    } else {
        gtk_label_set_text(GTK_LABEL(app->calc_result), "= —");
        status(app, "Use a simple expression such as 12 * 4.");
    }
}

static GtkWidget *build_calculator(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_box_append(GTK_BOX(box), heading("FALIZ Calculator", "Fast arithmetic with a clear keyboard-first input. Supports +, -, * and /."));
    app->calc_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->calc_entry), "12 * 4");
    g_signal_connect(app->calc_entry, "activate", G_CALLBACK(calculate_clicked), app);
    gtk_box_append(GTK_BOX(box), app->calc_entry);
    app->calc_result = gtk_label_new("= —");
    gtk_widget_add_css_class(app->calc_result, "title-1");
    gtk_widget_set_halign(app->calc_result, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), app->calc_result);
    gtk_box_append(GTK_BOX(box), button("Calculate", G_CALLBACK(calculate_clicked), app));
    return box;
}

static void screenshot_interactive(GtkButton *button, gpointer user_data) {
    (void)button;
    launch_command(user_data, "gnome-screenshot -i", "Screenshot selection started. Choose a window or area.");
}

static void screenshot_full(GtkButton *button, gpointer user_data) {
    (void)button;
    FalizApp *app = user_data;
    char *dir = g_build_filename(g_get_home_dir(), "Pictures", "FALIZ Screenshots", NULL);
    g_mkdir_with_parents(dir, 0755);
    char *path = g_build_filename(dir, "screenshot.png", NULL);
    char *command = g_strdup_printf("gnome-screenshot -f %s", path);
    launch_command(app, command, "Screenshot saved to Pictures/FALIZ Screenshots/screenshot.png.");
    g_free(command); g_free(path); g_free(dir);
}

static GtkWidget *build_screenshot(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_box_append(GTK_BOX(box), heading("FALIZ Screenshot", "Capture a selected area or save a full-screen image to a dedicated FALIZ folder."));
    gtk_box_append(GTK_BOX(box), button("Select area or window", G_CALLBACK(screenshot_interactive), app));
    gtk_box_append(GTK_BOX(box), button("Capture full screen", G_CALLBACK(screenshot_full), app));
    return box;
}

static gboolean update_monitor(gpointer user_data) {
    FalizApp *app = user_data;
    FILE *load = fopen("/proc/loadavg", "r");
    FILE *mem = fopen("/proc/meminfo", "r");
    double one = 0;
    unsigned long total = 0, available = 0;
    if (load) { int scanned = fscanf(load, "%lf", &one); (void)scanned; fclose(load); }
    if (mem) {
        char key[64]; unsigned long value; char unit[16];
        while (fscanf(mem, "%63s %lu %15s", key, &value, unit) == 3) {
            if (g_str_equal(key, "MemTotal:")) total = value;
            if (g_str_equal(key, "MemAvailable:")) available = value;
        }
        fclose(mem);
    }
    struct statvfs disk;
    unsigned long long disk_total = 0, disk_free = 0;
    if (statvfs(g_get_home_dir(), &disk) == 0) {
        disk_total = (unsigned long long)disk.f_blocks * disk.f_frsize;
        disk_free = (unsigned long long)disk.f_bavail * disk.f_frsize;
    }
    double used = total ? 100.0 * (double)(total - available) / (double)total : 0;
    double disk_used = disk_total ? 100.0 * (double)(disk_total - disk_free) / (double)disk_total : 0;
    char *text = g_strdup_printf("CPU load (1 min)   %.2f\nMemory             %.1f%% used\nHome storage       %.1f%% used\n\nLive values read from /proc and the home filesystem.", one, used, disk_used);
    gtk_label_set_text(GTK_LABEL(app->monitor_label), text);
    g_free(text);
    return G_SOURCE_CONTINUE;
}

static GtkWidget *build_monitor(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_box_append(GTK_BOX(box), heading("FALIZ System Monitor", "A lightweight view of live CPU load, memory pressure, and home storage."));
    app->monitor_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(app->monitor_label), 0);
    gtk_widget_add_css_class(app->monitor_label, "monospace");
    gtk_box_append(GTK_BOX(box), app->monitor_label);
    update_monitor(app);
    g_timeout_add_seconds(2, update_monitor, app);
    return box;
}

static void store_open(GtkButton *button, gpointer user_data) {
    (void)button;
    launch_command(user_data, "gnome-software", "FALIZ Store opened. Package installation remains managed by Software and PolicyKit.");
}

static GtkWidget *build_store(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_box_append(GTK_BOX(box), heading("FALIZ Software Store", "A safe front door to Debian and Flatpak software. Review package details before installing."));
    GtkWidget *search = gtk_search_entry_new();
    gtk_widget_set_sensitive(search, FALSE);
    gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search catalog in Software…");
    gtk_box_append(GTK_BOX(box), search);
    gtk_box_append(GTK_BOX(box), button("Open software catalog", G_CALLBACK(store_open), app));
    GtkWidget *note = gtk_label_new("FALIZ delegates privileged package transactions to the existing GNOME Software and PolicyKit services; no silent installation is performed here.");
    gtk_label_set_wrap(GTK_LABEL(note), TRUE);
    gtk_widget_set_halign(note, GTK_ALIGN_START);
    gtk_widget_add_css_class(note, "dim-label");
    gtk_box_append(GTK_BOX(box), note);
    return box;
}

static void media_open(GtkButton *button, gpointer user_data) {
    (void)button;
    launch_command(user_data, "vlc", "FALIZ Media Player opened. Select a local file or stream to play.");
}

static GtkWidget *build_media(FalizApp *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_box_append(GTK_BOX(box), heading("FALIZ Media Player", "A focused media entry point using VLC for broad local and network format support."));
    gtk_box_append(GTK_BOX(box), button("Open Media Player", G_CALLBACK(media_open), app));
    GtkWidget *note = gtk_label_new("Playback is handed to VLC, which is included in the FALIZ image and handles codecs, subtitles, playlists, and streams.");
    gtk_label_set_wrap(GTK_LABEL(note), TRUE);
    gtk_widget_set_halign(note, GTK_ALIGN_START);
    gtk_widget_add_css_class(note, "dim-label");
    gtk_box_append(GTK_BOX(box), note);
    return box;
}

static AppMode mode_from_args(int argc, char **argv) {
    const char *mode = argc > 2 && g_str_equal(argv[1], "--app") ? argv[2] : "files";
    if (g_str_equal(mode, "store")) return APP_STORE;
    if (g_str_equal(mode, "notes")) return APP_NOTES;
    if (g_str_equal(mode, "calculator")) return APP_CALCULATOR;
    if (g_str_equal(mode, "screenshot")) return APP_SCREENSHOT;
    if (g_str_equal(mode, "monitor")) return APP_MONITOR;
    if (g_str_equal(mode, "media")) return APP_MEDIA;
    return APP_FILES;
}

static const char *mode_title(AppMode mode) {
    switch (mode) {
        case APP_STORE: return "FALIZ Software Store";
        case APP_NOTES: return "FALIZ Notes";
        case APP_CALCULATOR: return "FALIZ Calculator";
        case APP_SCREENSHOT: return "FALIZ Screenshot";
        case APP_MONITOR: return "FALIZ System Monitor";
        case APP_MEDIA: return "FALIZ Media Player";
        default: return "FALIZ Files";
    }
}

static void activate(GApplication *application, gpointer user_data) {
    FalizApp *app = user_data;
    app->window = adw_application_window_new(GTK_APPLICATION(application));
    gtk_window_set_title(GTK_WINDOW(app->window), mode_title(app->mode));
    gtk_window_set_default_size(GTK_WINDOW(app->window), 820, 600);
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = adw_header_bar_new();
    GtkWidget *window_title = adw_window_title_new(mode_title(app->mode), "FALIZ OS BUILD 04.1");
    adw_header_bar_set_title_widget(ADW_HEADER_BAR(header), window_title);
    gtk_box_append(GTK_BOX(root), header);
    app->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_margin_top(app->body, 28); gtk_widget_set_margin_bottom(app->body, 22);
    gtk_widget_set_margin_start(app->body, 30); gtk_widget_set_margin_end(app->body, 30);
    gtk_widget_set_vexpand(app->body, TRUE);
    GtkWidget *content = NULL;
    switch (app->mode) {
        case APP_STORE: content = build_store(app); break;
        case APP_NOTES: content = build_notes(app); break;
        case APP_CALCULATOR: content = build_calculator(app); break;
        case APP_SCREENSHOT: content = build_screenshot(app); break;
        case APP_MONITOR: content = build_monitor(app); break;
        case APP_MEDIA: content = build_media(app); break;
        default: content = build_files(app); break;
    }
    gtk_box_append(GTK_BOX(app->body), content);
    gtk_box_append(GTK_BOX(root), app->body);
    app->status = gtk_label_new("");
    gtk_widget_set_halign(app->status, GTK_ALIGN_START);
    gtk_widget_set_margin_start(app->status, 30); gtk_widget_set_margin_end(app->status, 30);
    gtk_widget_set_margin_bottom(app->status, 16);
    gtk_widget_add_css_class(app->status, "dim-label");
    gtk_box_append(GTK_BOX(root), app->status);
    gtk_window_set_child(GTK_WINDOW(app->window), root);
    gtk_window_present(GTK_WINDOW(app->window));
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (g_str_equal(argv[i], "--version")) { g_print("%s\n", VERSION); return 0; }
    }
    FalizApp app = {.mode = mode_from_args(argc, argv), .current_dir = NULL};
    AdwApplication *application = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(activate), &app);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_free(app.current_dir);
    g_object_unref(application);
    return result;
}
