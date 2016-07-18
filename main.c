#include <gtk/gtk.h>
#include <readCrontab.h>
#include <cron_gui.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    main_window = openGTKWindow(NULL, "main_window.glade", "main_window");
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "cronjob.advanced"));
	gtk_builder_connect_signals(builder, NULL);

	GtkWidget *alligment = gtk_bin_get_child(GTK_BIN(main_window));
	GtkWidget *box = gtk_bin_get_child(GTK_BIN(alligment));
	printf("debug3 \n");
	//gtk_container_add(box,fragment);
	GList *children = gtk_container_get_children(GTK_CONTAINER(box));
	while ((children = g_list_next(children)) != NULL)
	{
		if (g_strcasecmp(gtk_widget_get_name(children->data), "GtkBox") == 0)
		{
		    read_cron_tab(children->data);
			printf("%s\n",gtk_widget_get_name(children->data));

			//gtk_container_add(children->data,fragment);
			printf("%s\n",gtk_widget_get_name(children->data));
			//printf("YAYYYYYY\n");
		}
	}

	g_object_unref(builder);
    gtk_main();
    return 0;
}

