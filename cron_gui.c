#include "cron_gui.h"

int lineNumberGUI = 1;

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

void fillFragmentLineNumber(GtkLabel *lineNumberLabel)
{
	int length = snprintf( NULL, 0, "%d", lineNumberGUI );
	char* lineNumberStr = malloc( length + 1 );
	snprintf( lineNumberStr, length + 1, "%d", lineNumberGUI );
	gtk_label_set_text(lineNumberLabel,lineNumberStr);
	free(lineNumberStr);
	lineNumberGUI++;
}

void addComment(GtkWidget *mainTable, const char *comment)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "comment"));
	GtkWidget *textbox = gtk_builder_get_object (builder,"comment_textbox");
	gtk_entry_set_text(textbox,comment);
	GtkWidget *lineNumberLabel = gtk_builder_get_object (builder,"comment_lineNumber");
	fillFragmentLineNumber(lineNumberLabel);
	gtk_builder_connect_signals(builder, NULL);
	gtk_container_add(mainTable,fragment);
	g_object_unref(builder);
}

void addVariable(GtkWidget *mainTable, const char *text)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "variable"));
	GtkWidget *textbox = gtk_builder_get_object (builder,"variable_textbox");
	gtk_entry_set_text(textbox,text);
	GtkWidget *lineNumberLabel = gtk_builder_get_object (builder,"variable_lineNumber");
	fillFragmentLineNumber(lineNumberLabel);
	gtk_builder_connect_signals(builder, NULL);
	gtk_container_add(mainTable,fragment);
	g_object_unref(builder);
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
