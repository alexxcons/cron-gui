#include <gtk/gtk.h>
#include <readCrontab.h>
#include <cron_gui.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
    main_window = openGTKWindow(builder, NULL, "main_window");
    GtkWidget *mainTable = gtk_builder_get_object (builder,"mainTable");
    initSizeGroups();
    read_cron_tab(mainTable);
	gtk_builder_connect_signals(builder, NULL);

    gtk_main();
	g_object_unref(builder);
    return 0;
}

