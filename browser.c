#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static GtkWidget *url_entry;
static GtkWidget *web_view;
static GtkWidget *back_button;
static GtkWidget *forward_button;
static GtkWidget *refresh_button;

void update_navigation_buttons(WebKitWebView *view) {
    gtk_widget_set_sensitive(back_button, webkit_web_view_can_go_back(view));
    gtk_widget_set_sensitive(forward_button, webkit_web_view_can_go_forward(view));
}

void load_url(GtkWidget *widget, gpointer data) {
    const gchar *url = gtk_entry_get_text(GTK_ENTRY(url_entry));
    
    if (!g_str_has_prefix(url, "http://") && !g_str_has_prefix(url, "https://")) {
        gchar *full_url = g_strdup_printf("https://%s", url);
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), full_url);
        g_free(full_url);
    } else {
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), url);
    }
}

void on_load_changed(WebKitWebView *view, WebKitLoadEvent load_event, gpointer data) {
    if (load_event == WEBKIT_LOAD_FINISHED) {
        const gchar *uri = webkit_web_view_get_uri(view);
        gtk_entry_set_text(GTK_ENTRY(url_entry), uri ? uri : "");
        update_navigation_buttons(view);
    }
}

void go_back(GtkWidget *widget, gpointer data) {
    webkit_web_view_go_back(WEBKIT_WEB_VIEW(web_view));
}

void go_forward(GtkWidget *widget, gpointer data) {
    webkit_web_view_go_forward(WEBKIT_WEB_VIEW(web_view));
}

void refresh_page(GtkWidget *widget, gpointer data) {
    webkit_web_view_reload(WEBKIT_WEB_VIEW(web_view));
}

// Handle key press in URL entry (Enter key)
gboolean on_url_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        load_url(widget, data);
        return TRUE;
    }
    return FALSE;
}

GtkWidget* create_browser_window(void) {
    // Create main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "TheGreen's web brawser");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Connect window destroy event
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create vertical box
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create navigation toolbar
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    // Back button
    back_button = gtk_button_new_with_label("←");
    gtk_widget_set_tooltip_text(back_button, "Back");
    g_signal_connect(back_button, "clicked", G_CALLBACK(go_back), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);

    // Forward button
    forward_button = gtk_button_new_with_label("→");
    gtk_widget_set_tooltip_text(forward_button, "Forward");
    g_signal_connect(forward_button, "clicked", G_CALLBACK(go_forward), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);

    // Refresh button
    refresh_button = gtk_button_new_with_label("⟲");
    gtk_widget_set_tooltip_text(refresh_button, "Refresh");
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh_page), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), refresh_button, FALSE, FALSE, 0);

    // URL entry
    url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(url_entry), "Enter URL (e.g., google.com)");
    g_signal_connect(url_entry, "key-press-event", G_CALLBACK(on_url_key_press), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), url_entry, TRUE, TRUE, 0);

    // Go button
    GtkWidget *go_button = gtk_button_new_with_label("Go");
    g_signal_connect(go_button, "clicked", G_CALLBACK(load_url), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), go_button, FALSE, FALSE, 0);

    // Create web view
    web_view = webkit_web_view_new();
    g_signal_connect(web_view, "load-changed", G_CALLBACK(on_load_changed), NULL);
    
    // Add scrolled window for web view
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), web_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Load initial page
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "https://www.google.com");
    gtk_entry_set_text(GTK_ENTRY(url_entry), "https://www.google.com");

    return window;
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create and show browser window
    GtkWidget *window = create_browser_window();
    gtk_widget_show_all(window);

    // Start main loop
    gtk_main();

    return 0;
}
