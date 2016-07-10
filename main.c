#include <gtk/gtk.h>
#include <readCrontab.h>

GtkWidget *main_window;

GtkWidget* openGTKWindow(GtkWidget *parent, char* file, char* windowName)
{
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, file, NULL);
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, windowName));
    if( parent != NULL )
    	gtk_window_set_transient_for (window,parent);
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(window);
    return window;
}

//void connectSignalsCronjobAdvanced(GtkWidget *cronjob)
//{
//	GList *children = gtk_container_get_children(GTK_CONTAINER(cronjob));
//	printf("connectSignalsCronjobAdvanced\n");
//	while ((children = g_list_next(children)) != NULL)
//	{
//		printf("%s\n",gtk_widget_get_name(children->data));
//		if (g_strcasecmp(gtk_widget_get_name(children->data), "GtkButton") == 0)
//		{
//			g_signal_connect(children->data, "clicked", G_CALLBACK(on_minute_pressed), NULL);
//			printf("g_signal_connect\n");
//		}
//	}
//}

int main(int argc, char *argv[])
{
    read_cron_tab();
    gtk_init(&argc, &argv);

    main_window = openGTKWindow(NULL, "main_window.glade", "main_window");
    printf("debug1 \n");
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "cronjob.advanced"));
	gtk_builder_connect_signals(builder, NULL);

	printf("debug2 \n");
	GtkWidget *alligment = gtk_bin_get_child(GTK_BIN(main_window));
	GtkWidget *box = gtk_bin_get_child(GTK_BIN(alligment));
	printf("debug3 \n");
	//gtk_container_add(box,fragment);
	GList *children = gtk_container_get_children(GTK_CONTAINER(box));
	while ((children = g_list_next(children)) != NULL)
	{
		if (g_strcasecmp(gtk_widget_get_name(children->data), "GtkBox") == 0)
		{
			printf("%s\n",gtk_widget_get_name(children->data));
			gtk_container_add(children->data,fragment);
			printf("%s\n",gtk_widget_get_name(children->data));
			//printf("YAYYYYYY\n");
		}
	}

	g_object_unref(builder);
    gtk_main();


    return 0;
}

// called when window is closed
void on_main_window_destroy()
{
	printf("debug");
    gtk_main_quit();
}

void on_minute_pressed(GtkWidget *button)
{
	printf("%s\n",gtk_widget_get_name(button));
    GtkWidget       *window = openGTKWindow(main_window, "minute_dialog.glade", "MinuteWizard");
}

void on_hour_pressed(GtkWidget *button)
{
	printf("hour pressed\n");
}

void on_dom_pressed(GtkWidget *button)
{
	printf("dom pressed\n");
}

void on_month_pressed(GtkWidget *button)
{
	printf("month pressed\n");
}

void on_dow_pressed(GtkWidget *button)
{
	printf("dow pressed\n");
}

