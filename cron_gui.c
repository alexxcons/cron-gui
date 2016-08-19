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

GtkSizeGroup * sizeGroupLineNumbers = NULL;
GtkSizeGroup * sizeGroupTimePickerBox = NULL;

void initSizeGroups()
{
	sizeGroupLineNumbers = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	sizeGroupTimePickerBox = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
}

void fillFragmentLineNumber(GtkLabel *lineNumberLabel)
{
	int length = snprintf( NULL, 0, "%d", lineNumberGUI );
	char* lineNumberStr = malloc( length + 1 );
	snprintf( lineNumberStr, length + 1, "%d", lineNumberGUI );
	gtk_label_set_text(lineNumberLabel,lineNumberStr);
	free(lineNumberStr);
	lineNumberGUI++;
	gtk_size_group_add_widget (sizeGroupLineNumbers,lineNumberLabel);
}

void addComment(GtkWidget *mainTable, const char *comment)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "comment"));
	GtkWidget *textbox = gtk_builder_get_object (builder,"comment_textbox");
	gtk_entry_set_text(textbox,comment);
	GtkWidget *lineNumberLabel = gtk_builder_get_object (builder,"comment_lineNumber");
	fillFragmentLineNumber(GTK_LABEL(lineNumberLabel));
	gtk_builder_connect_signals(builder, NULL);
	gtk_container_add(GTK_CONTAINER(mainTable),fragment);
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
	fillFragmentLineNumber(GTK_LABEL(lineNumberLabel));
	gtk_builder_connect_signals(builder, NULL);
	gtk_container_add(GTK_CONTAINER(mainTable),fragment);
	g_object_unref(builder);
}

void addSimpleCronJob(GtkWidget *mainTable, const char *simpleSelector, const char *command)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "cronjob.simple"));
	GtkWidget *commandBox = gtk_builder_get_object (builder,"simple_command");
	gtk_entry_set_text(commandBox,command);
	GtkWidget *timePickerBox = gtk_builder_get_object (builder,"simple_timePicker");
	gtk_size_group_add_widget (sizeGroupTimePickerBox,timePickerBox);
	GtkWidget *choiceBox = gtk_builder_get_object (builder,"simple_choice");
	int i;
	for( i = 0; i < SIMPLE_TIMING_VALUES_SIZE ;i++)
	{
		gtk_combo_box_text_append_text(choiceBox,simpleTimingValues[i]);
		if(strcmp(simpleTimingValues[i],simpleSelector) == 0)
		{
			gtk_combo_box_set_active (choiceBox,i);
		}
	}
	GtkWidget *lineNumberLabel = gtk_builder_get_object (builder,"simple_lineNumber");
	fillFragmentLineNumber(GTK_LABEL(lineNumberLabel));
	gtk_builder_connect_signals(builder, NULL);
	gtk_container_add(GTK_CONTAINER(mainTable),fragment);
	g_object_unref(builder);
}

void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);
	GtkWidget *fragment = GTK_WIDGET(gtk_builder_get_object(builder, "cronjob.advanced"));
	GtkWidget *commandBox = gtk_builder_get_object (builder,"advanced_command");
	gtk_entry_set_text(commandBox,command);
	GtkWidget *minuteBox = gtk_builder_get_object (builder,"advanced_minute");
	gtk_button_set_label (minuteBox,minute);
	GtkWidget *hourBox = gtk_builder_get_object (builder,"advanced_hour");
	gtk_button_set_label (hourBox,hour);
	GtkWidget *domBox = gtk_builder_get_object (builder,"advanced_dom");
	gtk_button_set_label (domBox,dom);
	GtkWidget *monthBox = gtk_builder_get_object (builder,"advanced_month");
	gtk_button_set_label (monthBox,month);
	GtkWidget *dowBox = gtk_builder_get_object (builder,"advanced_dow");
	gtk_button_set_label (dowBox,dow);
	GtkWidget *lineNumberLabel = gtk_builder_get_object (builder,"advanced_lineNumber");
	fillFragmentLineNumber(GTK_LABEL(lineNumberLabel));
	GtkWidget *timePickerBox = gtk_builder_get_object (builder,"advanced_timePicker");
	gtk_size_group_add_widget (sizeGroupTimePickerBox,timePickerBox);
	gtk_builder_connect_signals(builder, NULL);
	gtk_container_add(GTK_CONTAINER(mainTable),fragment);
	g_object_unref(builder);
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

// called when window is closed
void on_main_window_destroy()
{
	printf("debug");
    gtk_main_quit();
}

void on_loadWizard_pressed(GtkWidget *button)
{
	//printf("data: %i",data);
}

void on_cancel_pressed(GtkWidget *button, GtkWidget *wizardWindow)
{
	gtk_widget_destroy(wizardWindow);
}

void on_minute_pressed(GtkWidget *button)
{
	runWizard( MINUTE, main_window, button);
}

void on_hour_pressed(GtkWidget *button)
{
	runWizard( HOUR, main_window, button);
}

void on_dom_pressed(GtkWidget *button)
{
	runWizard( DOM, main_window, button);
}

void on_month_pressed(GtkWidget *button)
{
	runWizard( MONTH, main_window, button);
}

void on_dow_pressed(GtkWidget *button)
{
	runWizard( DOW, main_window, button);
}
