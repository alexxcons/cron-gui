#include <gtk/gtk.h>
#include <readCrontab.h>

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkWidget       *window;

    read_cron_tab();

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main_window.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

// called when window is closed
void on_main_window_destroy()
{
	printf("debug");
    gtk_main_quit();
}

