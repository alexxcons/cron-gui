// Copyright 2016 Alexander Schwinn
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "cron_gui.h"
#include "wizard.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

static int lineNumberGUI = 1;

const char * simpleTimingValues[] = {
    "@reboot",
    "@yearly",
    "@monthly",
    "@weekly",
    "@daily",
    "@hourly"
};
const int SIMPLE_TIMING_VALUES_SIZE	= 6;

static GtkSizeGroup * sizeGroupLineNumbers = NULL;
static GtkSizeGroup * sizeGroupTimePickerBox = NULL;

void initSizeGroups()
{
	sizeGroupLineNumbers = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	sizeGroupTimePickerBox = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
}

void addLineNumber(GtkWidget *lineBox)
{
	int length = snprintf( NULL, 0, "%d", lineNumberGUI );
	char* lineNumberStr = malloc( length + 1 );
	snprintf( lineNumberStr, length + 1, "%d", lineNumberGUI );
	GtkWidget *lineNumberLabel = gtk_label_new (lineNumberStr);
	gtk_container_add (GTK_CONTAINER (lineBox), lineNumberLabel);
	free(lineNumberStr);
	lineNumberGUI++;
	gtk_size_group_add_widget (sizeGroupLineNumbers,lineNumberLabel);
}

void activate_main_gui(GtkApplication *app, const char* fileToLoad)
{
	//	GtkCssProvider *cssProvider = gtk_css_provider_new ();
	//	gtk_css_provider_load_from_path(cssProvider,"./test.css",NULL);
	//	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	GtkWidget *window = gtk_application_window_new(app);
	g_signal_connect (window, "destroy", G_CALLBACK (on_main_window_destroy), NULL);
	main_window = GTK_WINDOW(window);

	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

	GtkWidget *main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add (GTK_CONTAINER (window), main_box);

	GtkWidget *menuBar = gtk_menu_bar_new();
	gtk_container_add (GTK_CONTAINER (main_box), menuBar);

	GtkWidget *lines = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add(GTK_CONTAINER (main_box), lines);

	GtkWidget *statusBar = gtk_statusbar_new();

	gtk_container_add (GTK_CONTAINER (main_box), statusBar);
	gtk_widget_show_all (window);

	initSizeGroups();

	if( fileToLoad == NULL)
		read_cron_tab(lines,"crontab");
	else
		read_cron_tab(lines,fileToLoad);

	gtk_main();
}

void addCommentOrVariable(GtkWidget *mainTable, const char *text)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (mainTable), box);

	addLineNumber(box);

	GtkWidget *textbox = gtk_entry_new ();
	gtk_container_add (GTK_CONTAINER (box), textbox);
	gtk_entry_set_text(textbox,text);
	gtk_entry_set_has_frame (textbox,FALSE);
	gtk_widget_set_hexpand (textbox,TRUE);

	gtk_widget_show_all (box);
}

void addSimpleCronJob(GtkWidget *mainTable, const char *simpleSelector, const char *command)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (mainTable), box);

	addLineNumber(box);

	GtkWidget *timePicker = gtk_combo_box_text_new ();
	gtk_container_add (GTK_CONTAINER (box), timePicker);
	gtk_size_group_add_widget (sizeGroupTimePickerBox,timePicker);
	int i;
	for( i = 0; i < SIMPLE_TIMING_VALUES_SIZE ;i++)
	{
		gtk_combo_box_text_append_text(timePicker,simpleTimingValues[i]);
		if(strcmp(simpleTimingValues[i],simpleSelector) == 0)
		{
			gtk_combo_box_set_active (timePicker,i);
		}
	}

	GtkWidget *commandBox = gtk_entry_new();
	gtk_container_add (GTK_CONTAINER (box), commandBox);
	gtk_entry_set_text(commandBox,command);
	gtk_widget_set_hexpand (commandBox,TRUE);
	gtk_widget_show_all (box);
}

void addTimeSelectorButton(GtkWidget *buttonBox, int type, const char* value )
{
	GtkWidget *button = gtk_button_new_with_label (value);
	gtk_container_add (GTK_CONTAINER (buttonBox), button);
	g_signal_connect (button, "clicked", G_CALLBACK (on_time_selector_pressed), type);
}

void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (mainTable), box);

	addLineNumber(box);

	GtkWidget *buttons = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (box), buttons);
	gtk_size_group_add_widget (sizeGroupTimePickerBox,buttons);

	addTimeSelectorButton(buttons,MINUTE,minute);
	addTimeSelectorButton(buttons,HOUR,hour);
	addTimeSelectorButton(buttons,DOM,dom);
	addTimeSelectorButton(buttons,MONTH,month);
	addTimeSelectorButton(buttons,DOW,dow);

	GtkWidget *commandBox = gtk_entry_new();
	gtk_container_add (GTK_CONTAINER (box), commandBox);
	gtk_entry_set_text(commandBox,command);
	gtk_widget_set_hexpand (commandBox,TRUE);
	gtk_widget_show_all (box);
}

void openWizardFromFile(char* fileName, char* wizardName)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, fileName, NULL);
    GtkWidget *widget= GTK_WIDGET(gtk_builder_get_object(builder, wizardName));
    gtk_window_set_transient_for (GTK_WINDOW(widget),main_window);
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show(widget);
    g_object_unref(builder);
}

void on_insert_line_activate(GtkWidget *button)
{
	openWizardFromFile("newLine.glade","newLine");
}

void on_main_window_destroy()
{
    gtk_main_quit();
}

void on_time_selector_pressed(GtkWidget *button, wizardType type)
{
	runWizard( type, main_window, button);
	//printf("data: %i",data);
}
