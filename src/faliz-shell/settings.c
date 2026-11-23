#include <adwaita.h>

#define APP_ID "com.faliz.OS.Settings"

static void activate(GApplication *application, gpointer user_data) {
    (void)user_data;
    AdwPreferencesWindow *window = ADW_PREFERENCES_WINDOW(adw_preferences_window_new());
    gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(application));
    gtk_window_set_title(GTK_WINDOW(window), "FALIZ Settings");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 620);

    AdwPreferencesPage *appearance = ADW_PREFERENCES_PAGE(adw_preferences_page_new());
    adw_preferences_page_set_title(appearance, "Appearance");
    adw_preferences_page_set_icon_name(appearance, "applications-graphics-symbolic");
    adw_preferences_window_add(window, appearance);

    AdwPreferencesGroup *theme_group = ADW_PREFERENCES_GROUP(adw_preferences_group_new());
    adw_preferences_group_set_title(theme_group, "Theme and desktop");
    adw_preferences_group_set_description(theme_group, "Personalize the FALIZ desktop surface.");
    adw_preferences_page_add(appearance, theme_group);

    AdwActionRow *dark_mode = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(dark_mode), "Dark theme");
    adw_action_row_set_subtitle(dark_mode, "Use graphite surfaces with purple and blue accents");
    GtkWidget *dark_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(dark_switch), TRUE);
    gtk_widget_set_valign(dark_switch, GTK_ALIGN_CENTER);
    adw_action_row_add_suffix(dark_mode, dark_switch);
    adw_preferences_group_add(theme_group, GTK_WIDGET(dark_mode));

    AdwActionRow *animations = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(animations), "Desktop animations");
    adw_action_row_set_subtitle(animations, "Animate launcher, dock, and workspace transitions");
    GtkWidget *animation_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(animation_switch), TRUE);
    gtk_widget_set_valign(animation_switch, GTK_ALIGN_CENTER);
    adw_action_row_add_suffix(animations, animation_switch);
    adw_preferences_group_add(theme_group, GTK_WIDGET(animations));

    AdwActionRow *wallpaper = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(wallpaper), "Wallpaper");
    adw_action_row_set_subtitle(wallpaper, "FALIZ Aurora — dark graphite gradient");
    adw_preferences_group_add(theme_group, GTK_WIDGET(wallpaper));

    AdwPreferencesPage *system = ADW_PREFERENCES_PAGE(adw_preferences_page_new());
    adw_preferences_page_set_title(system, "System");
    adw_preferences_page_set_icon_name(system, "preferences-system-symbolic");
    adw_preferences_window_add(window, system);

    AdwPreferencesGroup *session_group = ADW_PREFERENCES_GROUP(adw_preferences_group_new());
    adw_preferences_group_set_title(session_group, "FALIZ session");
    adw_preferences_group_set_description(session_group, "Current desktop integration status.");
    adw_preferences_page_add(system, session_group);

    AdwActionRow *compositor = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(compositor), "Wayland compositor");
    adw_action_row_set_subtitle(compositor, "sway — active BUILD 02 Wayland compositor");
    adw_action_row_add_suffix(compositor, gtk_image_new_from_icon_name("emblem-ok-symbolic"));
    adw_preferences_group_add(session_group, GTK_WIDGET(compositor));

    AdwActionRow *workspaces = ADW_ACTION_ROW(adw_action_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(workspaces), "Virtual workspaces");
    adw_action_row_set_subtitle(workspaces, "4 configured desktop workspace slots");
    adw_preferences_group_add(session_group, GTK_WIDGET(workspaces));

    AdwPreferencesGroup *hardware = ADW_PREFERENCES_GROUP(adw_preferences_group_new());
    adw_preferences_group_set_title(hardware, "Hardware services");
    adw_preferences_page_add(system, hardware);
    const char *services[][2] = {
        {"Network", "NetworkManager"},
        {"Audio", "PipeWire / WirePlumber"},
        {"Authorization", "Polkit"},
        {"Applications", "Flatpak ready"},
    };
    for (guint i = 0; i < G_N_ELEMENTS(services); i++) {
        AdwActionRow *row = ADW_ACTION_ROW(adw_action_row_new());
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), services[i][0]);
        adw_action_row_set_subtitle(row, services[i][1]);
        adw_action_row_add_suffix(row, gtk_image_new_from_icon_name("emblem-ok-symbolic"));
        adw_preferences_group_add(hardware, GTK_WIDGET(row));
    }

    AdwPreferencesGroup *operations = ADW_PREFERENCES_GROUP(adw_preferences_group_new());
    adw_preferences_group_set_title(operations, "Operations and safety");
    adw_preferences_group_set_description(operations, "Professional system controls with explicit confirmation safeguards.");
    adw_preferences_page_add(system, operations);
    const char *operations_list[][2] = {
        {"Lock screen", "swaylock protected session via Mod+Escape"},
        {"Update manager", "Read-only update checks; apply requires confirmation"},
        {"Installer", "Disk listing and dry-run foundation; no destructive actions"},
        {"Power controls", "Systemd shutdown, restart, logout, and suspend integration"},
    };
    for (guint i = 0; i < G_N_ELEMENTS(operations_list); i++) {
        AdwActionRow *row = ADW_ACTION_ROW(adw_action_row_new());
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), operations_list[i][0]);
        adw_action_row_set_subtitle(row, operations_list[i][1]);
        adw_preferences_group_add(operations, GTK_WIDGET(row));
    }

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    if (argc > 1 && g_strcmp0(argv[1], "--version") == 0) {
        g_print("FALIZ Settings BUILD 03 0.3.0\n");
        return 0;
    }
    AdwApplication *app = adw_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
