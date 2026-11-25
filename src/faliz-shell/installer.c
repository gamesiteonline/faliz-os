#include <adwaita.h>
#include <gtk/gtk.h>

#define APP_ID "com.faliz.OS.Installer"
#define INSTALLER_VERSION "FALIZ Installer BUILD 04 0.4.0"

typedef struct {
    AdwApplication *app;
    GtkWidget *window;
    GtkWidget *stack;
    GtkWidget *disk_combo;
    GtkWidget *name_entry;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *hostname_entry;
    GtkWidget *erase_check;
    GtkWidget *encrypt_check;
    GtkWidget *summary;
    GtkWidget *status;
    GtkWidget *back_button;
    GtkWidget *next_button;
    char *selected_target;
    int step;
} Installer;

static void set_status(Installer *installer, const char *message, gboolean error) {
    gtk_label_set_text(GTK_LABEL(installer->status), message);
    gtk_widget_remove_css_class(installer->status, error ? "success" : "error");
    gtk_widget_add_css_class(installer->status, error ? "error" : "success");
}

static GtkWidget *title_label(const char *title, const char *subtitle) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *heading = gtk_label_new(title);
    GtkWidget *detail = gtk_label_new(subtitle);
    gtk_widget_add_css_class(heading, "installer-title");
    gtk_widget_add_css_class(detail, "installer-subtitle");
    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_widget_set_halign(detail, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(detail), TRUE);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), detail);
    return box;
}

static GtkWidget *form_row(const char *label, GtkWidget *child) {
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *caption = gtk_label_new(label);
    gtk_widget_add_css_class(caption, "installer-caption");
    gtk_widget_set_halign(caption, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(row), caption);
    gtk_box_append(GTK_BOX(row), child);
    return row;
}

static void add_disk_lines(Installer *installer, const char *output) {
    const char *line = output;
    while (line && *line) {
        const char *end = strchr(line, '\n');
        gsize length = end ? (gsize)(end - line) : strlen(line);
        if (length > 0) {
            char *copy = g_strndup(line, length);
            if (!g_str_has_prefix(copy, "NAME")) {
                char *name = g_strstrip(copy);
                char *space = strchr(name, ' ');
                if (space) *space = '\0';
                if (*name && !g_str_equal(name, "loop0") && !g_str_has_prefix(name, "sr")) {
                    char *target = g_strdup_printf("/dev/%s", name);
                    gtk_string_list_append(GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(installer->disk_combo))), target);
                    g_free(target);
                }
            }
            g_free(copy);
        }
        line = end ? end + 1 : NULL;
    }
}

static void refresh_disks(GtkButton *button, gpointer user_data) {
    (void)button;
    Installer *installer = user_data;
    GtkStringList *model = GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(installer->disk_combo)));
    gtk_string_list_splice(model, 0, g_list_model_get_n_items(G_LIST_MODEL(model)), NULL);
    GError *error = NULL;
    char *stdout_text = NULL;
    char *stderr_text = NULL;
    if (!g_spawn_command_line_sync("lsblk -dn -o NAME,SIZE,TYPE,MODEL", &stdout_text, &stderr_text, NULL, &error)) {
        set_status(installer, "Could not inspect storage devices in this live session.", TRUE);
        g_clear_error(&error);
        g_free(stderr_text);
        return;
    }
    add_disk_lines(installer, stdout_text);
    if (g_list_model_get_n_items(G_LIST_MODEL(model)) == 0)
        set_status(installer, "No eligible disk was detected. Refresh after connecting a drive.", TRUE);
    else
        set_status(installer, "Storage devices refreshed. No disk has been changed.", FALSE);
    g_free(stdout_text);
    g_free(stderr_text);
}

static const char *selected_disk(Installer *installer) {
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(installer->disk_combo));
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(installer->disk_combo));
    return selected < g_list_model_get_n_items(model)
        ? gtk_string_list_get_string(GTK_STRING_LIST(model), selected) : NULL;
}

static void update_summary(Installer *installer) {
    const char *disk = selected_disk(installer);
    const char *name = gtk_editable_get_text(GTK_EDITABLE(installer->name_entry));
    const char *username = gtk_editable_get_text(GTK_EDITABLE(installer->username_entry));
    const char *hostname = gtk_editable_get_text(GTK_EDITABLE(installer->hostname_entry));
    gboolean erase = gtk_check_button_get_active(GTK_CHECK_BUTTON(installer->erase_check));
    gboolean encrypt = gtk_check_button_get_active(GTK_CHECK_BUTTON(installer->encrypt_check));
    char *text = g_strdup_printf(
        "Target disk: %s\nFull-disk erase: %s\nEncryption: %s\n\nUser: %s\nUsername: %s\nComputer name: %s\n\nBUILD 04 safety mode: this review is non-destructive. The current backend will only perform a dry run.",
        disk ? disk : "Not selected", erase ? "Acknowledged" : "Not selected", encrypt ? "Requested (not yet enabled)" : "Not requested",
        *name ? name : "Not set", *username ? username : "Not set", *hostname ? hostname : "Not set");
    gtk_label_set_text(GTK_LABEL(installer->summary), text);
    g_free(text);
}

static gboolean validate_step(Installer *installer) {
    if (installer->step == 0) {
        if (!selected_disk(installer)) {
            set_status(installer, "Select a target disk before continuing.", TRUE);
            return FALSE;
        }
        if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(installer->erase_check))) {
            set_status(installer, "A full-disk install requires acknowledging that the target will be erased.", TRUE);
            return FALSE;
        }
    } else if (installer->step == 1) {
        const char *name = gtk_editable_get_text(GTK_EDITABLE(installer->name_entry));
        const char *username = gtk_editable_get_text(GTK_EDITABLE(installer->username_entry));
        const char *password = gtk_editable_get_text(GTK_EDITABLE(installer->password_entry));
        if (!*name || !*username || !*password) {
            set_status(installer, "Enter your name, username, and password to continue.", TRUE);
            return FALSE;
        }
    }
    return TRUE;
}

static void show_step(Installer *installer, int step) {
    installer->step = step;
    const char *page = step == 0 ? "disk" : step == 1 ? "user" : step == 2 ? "review" : "complete";
    gtk_stack_set_visible_child_name(GTK_STACK(installer->stack), page);
    gtk_widget_set_sensitive(installer->back_button, step > 0 && step < 3);
    gtk_button_set_label(GTK_BUTTON(installer->next_button), step == 2 ? "Prepare safe dry run" : step == 3 ? "Close" : "Continue");
    if (step == 2) update_summary(installer);
    if (step == 3) gtk_widget_set_sensitive(installer->next_button, TRUE);
}

static void run_dry_run(Installer *installer) {
    const char *disk = selected_disk(installer);
    if (!disk) return;
    char *command = g_strdup_printf("faliz-installer --dry-run --target %s", disk);
    GError *error = NULL;
    char *stdout_text = NULL;
    char *stderr_text = NULL;
    gboolean ok = g_spawn_command_line_sync(command, &stdout_text, &stderr_text, NULL, &error);
    if (ok) {
        char *text = g_strdup_printf("Dry run complete.\n\n%s\n\nNo disk was modified. The full installer backend is not enabled yet.", stdout_text ? stdout_text : "FALIZ installation plan accepted.");
        GtkWidget *dialog = adw_message_dialog_new(GTK_WINDOW(installer->window), "Safe dry run complete", text);
        adw_message_dialog_add_response(ADW_MESSAGE_DIALOG(dialog), "close", "Close");
        adw_message_dialog_set_close_response(ADW_MESSAGE_DIALOG(dialog), "close");
        adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(installer->window));
        g_free(text);
        set_status(installer, "Plan checked safely. No disk was modified.", FALSE);
    } else {
        char *text = g_strdup_printf("The dry run could not be completed: %s", error ? error->message : "unknown error");
        set_status(installer, text, TRUE);
        g_free(text);
    }
    g_clear_error(&error);
    g_free(command);
    g_free(stdout_text);
    g_free(stderr_text);
}

static void next_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    Installer *installer = user_data;
    if (installer->step == 3) {
        gtk_window_close(GTK_WINDOW(installer->window));
        return;
    }
    if (!validate_step(installer)) return;
    if (installer->step == 2) {
        run_dry_run(installer);
        show_step(installer, 3);
        return;
    }
    show_step(installer, installer->step + 1);
}

static void back_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    Installer *installer = user_data;
    if (installer->step > 0) show_step(installer, installer->step - 1);
}

static GtkWidget *build_disk_page(Installer *installer) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_box_append(GTK_BOX(box), title_label("Choose where FALIZ OS will be installed", "This BUILD 04 preview identifies disks and prepares a safe installation plan. It never writes to a disk."));
    GtkStringList *model = gtk_string_list_new(NULL);
    installer->disk_combo = gtk_drop_down_new(G_LIST_MODEL(model), NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(installer->disk_combo), TRUE);
    gtk_box_append(GTK_BOX(box), form_row("Target disk", installer->disk_combo));
    GtkWidget *refresh = gtk_button_new_with_label("Refresh storage devices");
    gtk_widget_set_halign(refresh, GTK_ALIGN_START);
    g_signal_connect(refresh, "clicked", G_CALLBACK(refresh_disks), installer);
    gtk_box_append(GTK_BOX(box), refresh);
    installer->erase_check = gtk_check_button_new_with_label("I understand that a future full install will erase the selected disk");
    gtk_box_append(GTK_BOX(box), installer->erase_check);
    installer->encrypt_check = gtk_check_button_new_with_label("Enable disk encryption (planned; not enabled in this preview)");
    gtk_widget_set_sensitive(installer->encrypt_check, FALSE);
    gtk_box_append(GTK_BOX(box), installer->encrypt_check);
    GtkWidget *note = gtk_label_new("Important: the final destructive installer is intentionally disabled until partitioning, encryption, bootloader installation, and recovery handling are fully validated.");
    gtk_label_set_wrap(GTK_LABEL(note), TRUE);
    gtk_widget_add_css_class(note, "installer-warning");
    gtk_box_append(GTK_BOX(box), note);
    return box;
}

static GtkWidget *build_user_page(Installer *installer) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_box_append(GTK_BOX(box), title_label("Create your FALIZ user", "These details will become part of the future installation plan. They are not written to disk by this preview."));
    installer->name_entry = gtk_entry_new();
    gtk_box_append(GTK_BOX(box), form_row("Full name", installer->name_entry));
    installer->username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(installer->username_entry), "fahad");
    gtk_box_append(GTK_BOX(box), form_row("Username", installer->username_entry));
    installer->password_entry = gtk_password_entry_new();
    gtk_box_append(GTK_BOX(box), form_row("Password", installer->password_entry));
    installer->hostname_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(installer->hostname_entry), "faliz-pc");
    gtk_box_append(GTK_BOX(box), form_row("Computer name", installer->hostname_entry));
    return box;
}

static GtkWidget *build_review_page(Installer *installer) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_box_append(GTK_BOX(box), title_label("Review your installation plan", "Check every detail before continuing. This preview will only invoke a guarded dry run."));
    installer->summary = gtk_label_new("");
    gtk_label_set_selectable(GTK_LABEL(installer->summary), TRUE);
    gtk_label_set_wrap(GTK_LABEL(installer->summary), TRUE);
    gtk_widget_set_halign(installer->summary, GTK_ALIGN_START);
    gtk_widget_add_css_class(installer->summary, "installer-summary");
    gtk_box_append(GTK_BOX(box), installer->summary);
    return box;
}

static GtkWidget *build_complete_page(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_box_append(GTK_BOX(box), title_label("FALIZ installation plan prepared", "The safe preview is complete. No partitions, filesystems, bootloaders, or disks were changed."));
    GtkWidget *done = gtk_label_new("BUILD 04 Installer Preview\n\nThe next implementation stage will connect this guided workflow to a fully tested installation backend. Until then, FALIZ remains a live desktop and this tool remains non-destructive by design.");
    gtk_label_set_wrap(GTK_LABEL(done), TRUE);
    gtk_widget_add_css_class(done, "installer-success");
    gtk_box_append(GTK_BOX(box), done);
    return box;
}

static void activate(GApplication *app, gpointer user_data) {
    Installer *installer = user_data;
    installer->app = ADW_APPLICATION(app);
    installer->window = adw_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(installer->window), "FALIZ Installer");
    gtk_window_set_default_size(GTK_WINDOW(installer->window), 760, 620);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(root, "installer-window");
    GtkWidget *header = adw_header_bar_new();
    adw_header_bar_set_title_widget(ADW_HEADER_BAR(header), gtk_label_new(INSTALLER_VERSION));
    gtk_box_append(GTK_BOX(root), header);

    installer->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(installer->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_widget_set_vexpand(installer->stack, TRUE);
    gtk_widget_set_margin_start(installer->stack, 36);
    gtk_widget_set_margin_end(installer->stack, 36);
    gtk_widget_set_margin_top(installer->stack, 30);
    gtk_widget_set_margin_bottom(installer->stack, 20);
    gtk_stack_add_named(GTK_STACK(installer->stack), build_disk_page(installer), "disk");
    gtk_stack_add_named(GTK_STACK(installer->stack), build_user_page(installer), "user");
    gtk_stack_add_named(GTK_STACK(installer->stack), build_review_page(installer), "review");
    gtk_stack_add_named(GTK_STACK(installer->stack), build_complete_page(), "complete");
    gtk_box_append(GTK_BOX(root), installer->stack);

    GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_start(footer, 36);
    gtk_widget_set_margin_end(footer, 36);
    gtk_widget_set_margin_bottom(footer, 24);
    installer->status = gtk_label_new("No disk has been changed.");
    gtk_label_set_wrap(GTK_LABEL(installer->status), TRUE);
    gtk_widget_set_halign(installer->status, GTK_ALIGN_START);
    gtk_widget_set_hexpand(installer->status, TRUE);
    gtk_box_append(GTK_BOX(footer), installer->status);
    installer->back_button = gtk_button_new_with_label("Back");
    g_signal_connect(installer->back_button, "clicked", G_CALLBACK(back_clicked), installer);
    gtk_box_append(GTK_BOX(footer), installer->back_button);
    installer->next_button = gtk_button_new_with_label("Continue");
    gtk_widget_add_css_class(installer->next_button, "suggested-action");
    g_signal_connect(installer->next_button, "clicked", G_CALLBACK(next_clicked), installer);
    gtk_box_append(GTK_BOX(footer), installer->next_button);
    gtk_box_append(GTK_BOX(root), footer);

    adw_application_window_set_content(ADW_APPLICATION_WINDOW(installer->window), root);
    show_step(installer, 0);
    refresh_disks(NULL, installer);
    gtk_window_present(GTK_WINDOW(installer->window));
}

int main(int argc, char **argv) {
    if (argc > 1 && g_strcmp0(argv[1], "--version") == 0) {
        g_print("%s\n", INSTALLER_VERSION);
        return 0;
    }
    Installer installer = {0};
    AdwApplication *app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &installer);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
