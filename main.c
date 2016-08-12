#include <gtk/gtk.h>
#include <readCrontab.h>
#include <cron_gui.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

	// load css file
	GtkCssProvider *cssProvider = gtk_css_provider_new ();
	gtk_css_provider_load_from_path(cssProvider,"./test.css",NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show(window);
    main_window = GTK_WINDOW(window);

    GtkWidget *mainTable = gtk_builder_get_object (builder,"mainTable");
    initSizeGroups();
    read_cron_tab(mainTable);
	gtk_builder_connect_signals(builder, NULL);

    gtk_main();
	g_object_unref(builder);
    return 0;
}

