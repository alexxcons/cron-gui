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
#include <readCrontab.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

const char * simpleTimingValues[] = {
    "@reboot",
    "@yearly",
    "@monthly",
    "@weekly",
    "@daily",
    "@hourly"
};

const int SIMPLE_TIMING_VALUES_SIZE	= 6;

static GtkSizeGroup * sizeGroupTimePickerBox = NULL;

const char* commentOrVariableFragmentName	= "commentOrVariable";
const char* simpleJobFragmentName			= "simpleJob";
const char* advancedJobFragmentName			= "advancedJob";

const char* DEFAULT_CRONTAB_PATH = "/etc/crontab";

const char* TEMP_FILE_NAME_TEMPLATE = "crontab-gui-XXXXXX";

void initSizeGroups()
{
	sizeGroupTimePickerBox = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
}

void add_toolbar_item(const context_base* context, const char* actionName, const GtkCallback* callback, void * parameter)
{
	GtkWidget *extendedEditor_toolbox = get_extendedEditor_toolbox_from_notebook(context->notebook);
	GtkWidget *toolbarItem = gtk_button_new_with_label (actionName);
	gtk_container_add (GTK_CONTAINER (extendedEditor_toolbox), toolbarItem);
	g_signal_connect(G_OBJECT(toolbarItem), "clicked", G_CALLBACK(callback), parameter);
}

void activate_main_gui(GtkApplication *app, context_base* context)
{
	printf("activate_main_gui 1\n");

	context->cb_extended2plain = extended2plain;
	context->cb_plain2extended = plain2extended;
	activate_main_gui_base(app, context);
	add_toolbar_item(context, "Add Simple Job", on_add_simple_job, context);
	add_toolbar_item(context, "Add Advanced Job", on_add_advanced_job, context);
	add_toolbar_item(context, "Add Comment", on_add_comment, context);
	printf("activate_main_gui 2\n");
	gtk_widget_show_all (context->window);
	printf("activate_main_gui 3\n");
	initSizeGroupsBase(context);
	initSizeGroups();
	printf("activate_main_gui 4\n");
	if( context->fileToLoadAtStartup == NULL)
	{
		reportInfo("No file selected - attempt to load default crontab from",DEFAULT_CRONTAB_PATH, context);
		context->fileToLoadAtStartup = strdup(DEFAULT_CRONTAB_PATH);
	}
	printf("activate_main_gui 5\n");
	loadFile(context,context->fileToLoadAtStartup);
	printf("activate_main_gui 6\n");
	gtk_main();
}

void addCommentOrVariable(const char *text, context_base* context)
{
	printf("addCommentOrVariable 1\n");
	GtkWidget *extendedEditor_linebox = get_extendedEditor_linebox_from_notebook(context->notebook);
	printf("addCommentOrVariable 2\n");
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_set_name (box,commentOrVariableFragmentName);
	setDragDestination(extendedEditor_linebox, box);
	gtk_container_add (GTK_CONTAINER (extendedEditor_linebox), box);

	addLineNumber(extendedEditor_linebox, box, context);

	GtkWidget *textbox = gtk_entry_new();
	setDragDestination(extendedEditor_linebox, textbox);
	gtk_container_add (GTK_CONTAINER (box), textbox);
	gtk_entry_set_text(GTK_ENTRY(textbox),text);
	gtk_entry_set_has_frame (GTK_ENTRY(textbox),FALSE);
	gtk_widget_set_hexpand (textbox,TRUE);

	GtkWidget *dragButton = addDragDropButton(box,extendedEditor_linebox);
	gtk_entry_set_has_frame (GTK_ENTRY(dragButton),FALSE);
	gtk_widget_show_all (box);
}

void addSimpleCronJob(const char *simpleSelector, const char *command, context_base* context)
{
	GtkWidget *extendedEditor_linebox = get_extendedEditor_linebox_from_notebook(context->notebook);
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_set_name (box,simpleJobFragmentName);
	gtk_container_add (GTK_CONTAINER (extendedEditor_linebox), box);
	setDragDestination(extendedEditor_linebox, box);

	addLineNumber(extendedEditor_linebox, box, context);

	GtkWidget *timePicker = gtk_combo_box_text_new ();
	gtk_container_add (GTK_CONTAINER (box), timePicker);
	gtk_size_group_add_widget (sizeGroupTimePickerBox,timePicker);
	int i;
	for( i = 0; i < SIMPLE_TIMING_VALUES_SIZE ;i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(timePicker),simpleTimingValues[i]);
		if(strcmp(simpleTimingValues[i],simpleSelector) == 0)
		{
			gtk_combo_box_set_active (GTK_COMBO_BOX(timePicker),i);
		}
	}

	GtkWidget *commandBox = gtk_entry_new();
	gtk_container_add (GTK_CONTAINER (box), commandBox);
	setDragDestination(extendedEditor_linebox, commandBox);
	gtk_entry_set_text(GTK_ENTRY(commandBox),command);
	gtk_widget_set_hexpand (commandBox,TRUE);
	guint16 textLength = gtk_entry_get_text_length(commandBox);
	gtk_entry_set_width_chars (commandBox,textLength);
	g_signal_connect (commandBox, "changed", G_CALLBACK (on_entry_changed), context);

	addDragDropButton(box,extendedEditor_linebox);
	gtk_widget_show_all (box);
}

void simpleCronJob2Text(GtkWidget *simpleCronJob, GtkWidget *plainTextEditor_textView)
{
	GList *element_lineNumber = gtk_container_get_children(GTK_CONTAINER(simpleCronJob));
	GList *element_timePicker = g_list_next(element_lineNumber);
	GList *element_command = g_list_next(element_timePicker);
	GtkWidget* timePicker =  element_timePicker->data;
	GtkWidget* command =  element_command->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(timePicker)));
	plainTextEditor_textView_append(plainTextEditor_textView," ");
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_entry_get_text(GTK_ENTRY(command)));
	plainTextEditor_textView_append(plainTextEditor_textView,"\n");
}

void addTimeSelectorButton(GtkWidget *buttonBox, const gchar *name, const char* value, context_base* context )
{
	GtkWidget *button = gtk_button_new_with_label (value);
	gtk_container_add (GTK_CONTAINER (buttonBox), button);
	gtk_widget_set_name (button, name);
	g_signal_connect (button, "clicked", G_CALLBACK (on_time_selector_pressed), context);
}

void addAdvancedCronJob(const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command, context_base* context)
{
	GtkWidget *extendedEditor_linebox = get_extendedEditor_linebox_from_notebook(context->notebook);
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_set_name (box,advancedJobFragmentName);
	gtk_container_add (GTK_CONTAINER (extendedEditor_linebox), box);
	setDragDestination(extendedEditor_linebox, box);

	addLineNumber(extendedEditor_linebox, box, context);

	GtkWidget *buttons = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (box), buttons);
	gtk_size_group_add_widget (sizeGroupTimePickerBox,buttons);

	addTimeSelectorButton(buttons,buttonName[MINUTE],minute, context);
	addTimeSelectorButton(buttons,buttonName[HOUR],hour, context);
	addTimeSelectorButton(buttons,buttonName[DOM],dom, context);
	addTimeSelectorButton(buttons,buttonName[MONTH],month, context);
	addTimeSelectorButton(buttons,buttonName[DOW],dow, context);

	GtkWidget *commandBox = gtk_entry_new();
	setDragDestination(extendedEditor_linebox, commandBox);
	gtk_container_add (GTK_CONTAINER (box), commandBox);
	gtk_entry_set_text(GTK_ENTRY(commandBox),command);
	gtk_widget_set_hexpand (commandBox,TRUE);
	guint16 textLength = gtk_entry_get_text_length(commandBox);
	gtk_entry_set_width_chars (commandBox,textLength);
	g_signal_connect (commandBox, "changed", G_CALLBACK (on_entry_changed), context);

	addDragDropButton(box,extendedEditor_linebox);
	gtk_widget_show_all (box);
}

void advancedCronJob2Text(GtkWidget *advancedCronJob, GtkWidget *plainTextEditor_textView)
{
	GList *element_lineNumber = gtk_container_get_children(GTK_CONTAINER(advancedCronJob));
	GList *element_buttons = g_list_next(element_lineNumber);
	GtkWidget* timeSelector;
	GList *element_minute = gtk_container_get_children(GTK_CONTAINER(element_buttons->data));
	timeSelector =  element_minute->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	plainTextEditor_textView_append(plainTextEditor_textView," ");
	GList *element_hour = g_list_next(element_minute);
	timeSelector =  element_hour->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	plainTextEditor_textView_append(plainTextEditor_textView," ");
	GList *element_dom = g_list_next(element_hour);
	timeSelector =  element_dom->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	plainTextEditor_textView_append(plainTextEditor_textView," ");
	GList *element_month = g_list_next(element_dom);
	timeSelector =  element_month->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	plainTextEditor_textView_append(plainTextEditor_textView," ");
	GList *element_dow = g_list_next(element_month);
	timeSelector =  element_dow->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	plainTextEditor_textView_append(plainTextEditor_textView," ");

	GList *element_command = g_list_next(element_buttons);
	GtkWidget* command =  element_command->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_entry_get_text(GTK_ENTRY(command)));

	plainTextEditor_textView_append(plainTextEditor_textView,"\n");
}

void on_add_comment(GtkWidget *source, context_base* context )
{
	addCommentOrVariable("# Put comment here", context);
}

void on_add_simple_job(GtkWidget *source, context_base* context )
{
	addSimpleCronJob(simpleTimingValues[0], "command to execute", context);
}

void on_add_advanced_job(GtkWidget *source, context_base* context )
{
	addAdvancedCronJob("0", "0", "*", "*", "*", "command to execute", context);
}

void on_time_selector_pressed(GtkWidget *button, context_base* context)
{
	runWizard( context->main_window, button);
}

void on_entry_changed (GtkEditable* entry, context_base* context)
{
	guint16 textLength = gtk_entry_get_text_length(GTK_ENTRY(entry));
	printf("%i\n",textLength);
	gtk_entry_set_width_chars (GTK_ENTRY(entry),textLength);
}

void extended2plain(void* context)
{
	context_base* ctx = (context_base*)context;
	GtkWidget* plainTextEditor_textView = get_plainTextEditor_textView_from_notebook(ctx->notebook);
	plainTextEditor_textView_clear(plainTextEditor_textView);
	GList *lines = gtk_container_get_children(GTK_CONTAINER(get_extendedEditor_linebox_from_notebook(ctx->notebook)));
	for( GList *line = lines; line != NULL; line = g_list_next(line) )
	{
		const char* lineType = gtk_widget_get_name(line->data);
		if(  strcmp(lineType,commentOrVariableFragmentName) == 0 )
		{
			extendedEditorCommentLine2Text(line->data,plainTextEditor_textView);
		}
		else if(  strcmp(lineType,simpleJobFragmentName) == 0 )
		{
			simpleCronJob2Text(line->data,plainTextEditor_textView);
		}
		else if(  strcmp(lineType,advancedJobFragmentName) == 0 )
		{
			advancedCronJob2Text(line->data,plainTextEditor_textView);
		}
		else
		{
			reportError("Error: the following widget could not be converted to string",lineType, context);
			markErrorInLine(line); // TODO;
		}
	}
}

void plain2extended(void* context)
{
	context_base* ctx = (context_base*)context;
	GtkWidget *textView = get_plainTextEditor_textView_from_notebook(ctx->notebook);
	const gchar* string = plainTextEditor_textView_asString(textView);
//	printf("string: %s\n",string);
//	printf("sizeof(string): %i\n",strlen(string));
	if( strlen(string) < 1)
		return;

	char filePathBuff[1024];
	memset(filePathBuff,0,sizeof(filePathBuff));
	int* fd = -1;

	const char* SYSTEM_D_TEMP_FOLDER_VAR = "XDG_RUNTIME_DIR";
	const char* DEFAULT_TEMP_FOLDER = "/tmp";
	char* systemDTemp = getenv(SYSTEM_D_TEMP_FOLDER_VAR);
	if(systemDTemp)
	{
		strncpy(filePathBuff,systemDTemp,strlen(systemDTemp));
		strcat(filePathBuff,"/");
		strcat(filePathBuff,TEMP_FILE_NAME_TEMPLATE);
		fd = mkstemp(filePathBuff);
	}
	else
	{
		strncpy(filePathBuff,DEFAULT_TEMP_FOLDER,strlen(DEFAULT_TEMP_FOLDER));
		strcat(filePathBuff,"/");
		strcat(filePathBuff,TEMP_FILE_NAME_TEMPLATE);
		fd = mkstemp(filePathBuff);
	}
	errno = 0;
	unlink(filePathBuff); // whenever the file or the program is closed, the file is deleted.
	if(fd < 1)
	{
		reportError("Creation of temp file failed with error: ",strerror(errno), context);
		return;
	}

	FILE* fp = fdopen(fd, "w+");
	if(-1 == fprintf(fp,"%s",string))
	{
		printf("\n write failed with error [%s]\n",strerror(errno));
		return;
	}

	rewind(fp);
	extendedEditor_linebox_clear(get_extendedEditor_linebox_from_notebook(ctx->notebook));
	read_cron_tab(ctx,fp);
	fclose(fp);
}
