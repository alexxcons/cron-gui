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
#include <writeCrontab.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

const char * simpleTimingValues[] = {
    "@reboot",
    "@yearly",
    "@monthly",
    "@weekly",
    "@daily",
    "@hourly"
};

enum
{
  TARGET_WHATEVER,
};

static GtkTargetEntry targetentries[] =
{
  { "GtkBox",        0, TARGET_WHATEVER },
  { "STRING",        0, TARGET_WHATEVER },
  { "GtkImage",        0, TARGET_WHATEVER },
};

const gint nTargetEntries = 3;

const int SIMPLE_TIMING_VALUES_SIZE	= 6;

static GtkSizeGroup * sizeGroupLineNumbers = NULL;
static GtkSizeGroup * sizeGroupTimePickerBox = NULL;

const char* commentOrVariableFragmentName	= "commentOrVariable";
const char* simpleJobFragmentName			= "simpleJob";
const char* advancedJobFragmentName			= "advancedJob";

char* expandString(char *string, const char *stringToAdd)
{
	if( stringToAdd == NULL )
	{
		return string;
	}
    const size_t sizeToAdd = strlen(stringToAdd);
	if( sizeToAdd == 0 )
	{
		return string;
	}
    char *newBuffer = NULL;
    if( string == NULL)
    {
    	newBuffer = malloc( ( sizeToAdd + 1)  * sizeof(*stringToAdd) );// + 1 for \0
        //printf("malloc\n");
    	newBuffer[0] = 0; // need to erase crap in first element, since we wanmt to strcat later
    }
    else
    {
    	const size_t sizeOld = strlen(string);
        const size_t sizeNew = sizeOld + sizeToAdd;
        //printf("sizeOld: %i\n",sizeOld);
        //printf("sizeNew: %i\n",sizeNew);
    	newBuffer = realloc( string, (sizeNew + 1)  * sizeof(*stringToAdd) );// + 1 for \0
        //printf("realloc\n");
    }

    if ( newBuffer == NULL )
    {
        printf("Unexpected null pointer when malloc/realloc.\n");
        exit(ERROR_EXIT);
    }
	strcat(newBuffer,stringToAdd);
    //printf("%s",newBuffer);
    return newBuffer;
}

void initSizeGroups()
{
	sizeGroupLineNumbers = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	sizeGroupTimePickerBox = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
}

void addLineNumber(GtkWidget *mainTable,GtkWidget *lineBox)
{
	GtkWidget *lineNumberLabel = gtk_label_new ("");
	gtk_container_add (GTK_CONTAINER (lineBox), lineNumberLabel);
	gtk_size_group_add_widget (sizeGroupLineNumbers,lineNumberLabel);
	fixLineNumbers(mainTable);
}

void setDragDestination(GtkWidget *mainTable, GtkWidget *widget)
{
	gtk_drag_dest_set (widget, GTK_DEST_DEFAULT_MOTION, targetentries, nTargetEntries, GDK_ACTION_MOVE);
	//g_signal_connect(widget, "drag-data-received",G_CALLBACK(on_drag_data_received),widget);
	g_signal_connect(widget, "drag-motion",G_CALLBACK(on_drag_motion),mainTable);
	g_signal_connect(widget, "drag-drop",G_CALLBACK(on_drag_drop),mainTable);
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

	GtkWidget *fileMenu = gtk_menu_new();
	GtkWidget *fileBase = gtk_menu_item_new_with_label("File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileBase), fileMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), fileBase);
	GtkWidget *fileNew = gtk_menu_item_new_with_label("New");
	GtkWidget *fileOpen = gtk_menu_item_new_with_label("Open");
	GtkWidget *fileSave = gtk_menu_item_new_with_label("Save");
	GtkWidget *fileSaveAs = gtk_menu_item_new_with_label("Save as");
	GtkWidget *fileClose = gtk_menu_item_new_with_label("Close");
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileNew);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileOpen);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileSave);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileSaveAs);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileClose);
	g_signal_connect(G_OBJECT(fileNew), "activate", G_CALLBACK(on_menu_new), lines);
	g_signal_connect(G_OBJECT(fileOpen), "activate", G_CALLBACK(on_menu_open), lines);
	g_signal_connect(G_OBJECT(fileSave), "activate", G_CALLBACK(on_menu_save), lines);
	g_signal_connect(G_OBJECT(fileSaveAs), "activate", G_CALLBACK(on_menu_save_as), lines);
	g_signal_connect(G_OBJECT(fileClose), "activate", G_CALLBACK(on_menu_close), lines);

	GtkWidget *editMenu = gtk_menu_new();
	GtkWidget *editBase = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(editBase), editMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), editBase);
	GtkWidget *editCopy = gtk_menu_item_new_with_label("Copy");
	GtkWidget *editPaste = gtk_menu_item_new_with_label("Paste");
	GtkWidget *editAddSimpleJob = gtk_menu_item_new_with_label("Add Simple Job");
	GtkWidget *editAddAdvancedJob = gtk_menu_item_new_with_label("Add Advanced Job");
	GtkWidget *editAddComment = gtk_menu_item_new_with_label("Add Comment/Variable");
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editCopy);
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editPaste);
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editAddSimpleJob);
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editAddAdvancedJob);
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editAddComment);
	g_signal_connect(G_OBJECT(editCopy), "activate", G_CALLBACK(on_menu_copy), lines);
	g_signal_connect(G_OBJECT(editPaste), "activate", G_CALLBACK(on_menu_paste), lines);
	g_signal_connect(G_OBJECT(editAddSimpleJob), "activate", G_CALLBACK(on_menu_add_simple_job), lines);
	g_signal_connect(G_OBJECT(editAddAdvancedJob), "activate", G_CALLBACK(on_menu_add_advanced_job), lines);
	g_signal_connect(G_OBJECT(editAddComment), "activate", G_CALLBACK(on_menu_add_comment), lines);

	GtkWidget *helpMenu = gtk_menu_new();
	GtkWidget *helpBase = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpBase), helpMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), helpBase);
	GtkWidget *helpIndex = gtk_menu_item_new_with_label("Index");
	GtkWidget *helpAbout = gtk_menu_item_new_with_label("About");
	gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), helpIndex);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), helpAbout);
	g_signal_connect(G_OBJECT(helpIndex), "activate", G_CALLBACK(on_menu_index), lines);
	g_signal_connect(G_OBJECT(helpAbout), "activate", G_CALLBACK(on_menu_about), lines);

	GtkWidget *toolbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,3);
	gtk_container_add (GTK_CONTAINER (main_box), toolbox);
	GtkWidget *addComment = gtk_button_new_with_label ("addComment");
	gtk_container_add (GTK_CONTAINER (toolbox), addComment);

	GtkWidget *trash = gtk_image_new_from_icon_name ("user-trash-symbolic",GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_set_name(trash,"trash");
	setDragDestination(lines,trash);
	gtk_widget_set_hexpand (trash,TRUE);
	gtk_container_add (GTK_CONTAINER (toolbox), trash);
	GtkWidget *addSimpleJob = gtk_button_new_with_label ("addSimpleJob");
	gtk_container_add (GTK_CONTAINER (toolbox), addSimpleJob);
	GtkWidget *addAdvancedJob = gtk_button_new_with_label ("addAdvancedJob");
	gtk_container_add (GTK_CONTAINER (toolbox), addAdvancedJob);

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
	gtk_widget_set_name (box,commentOrVariableFragmentName);
	setDragDestination(mainTable, box);
	gtk_container_add (GTK_CONTAINER (mainTable), box);

	addLineNumber(mainTable,box);

	GtkWidget *textbox = gtk_entry_new();
	setDragDestination(mainTable, textbox);
	gtk_container_add (GTK_CONTAINER (box), textbox);
	gtk_entry_set_text(GTK_ENTRY(textbox),text);
	gtk_entry_set_has_frame (GTK_ENTRY(textbox),FALSE);
	gtk_widget_set_hexpand (textbox,TRUE);

	GtkWidget *dragButton = addDragDropButton(box,mainTable);
	gtk_entry_set_has_frame (GTK_ENTRY(dragButton),FALSE);
	gtk_widget_show_all (box);
}

char* commentOrVariableToString(GtkWidget *commentOrVariable, char* string)
{
	GList *element_lineNumber = gtk_container_get_children(GTK_CONTAINER(commentOrVariable));
	GList *element_textBox = g_list_next(element_lineNumber);
	GtkWidget* textbox =  element_textBox->data;
	string = expandString(string,gtk_entry_get_text(GTK_ENTRY(textbox)));
	string = expandString(string,"\n");
	return string;
}

void addSimpleCronJob(GtkWidget *mainTable, const char *simpleSelector, const char *command)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_set_name (box,simpleJobFragmentName);
	gtk_container_add (GTK_CONTAINER (mainTable), box);
	setDragDestination(mainTable, box);

	addLineNumber(mainTable,box);

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
	setDragDestination(mainTable, commandBox);
	gtk_entry_set_text(GTK_ENTRY(commandBox),command);
	gtk_widget_set_hexpand (commandBox,TRUE);
	addDragDropButton(box,mainTable);
	gtk_widget_show_all (box);
}

char* simpleCronJobToString(GtkWidget *simpleCronJob, char* string)
{
	GList *element_lineNumber = gtk_container_get_children(GTK_CONTAINER(simpleCronJob));
	GList *element_timePicker = g_list_next(element_lineNumber);
	GList *element_command = g_list_next(element_timePicker);
	GtkWidget* timePicker =  element_timePicker->data;
	GtkWidget* command =  element_command->data;
	string = expandString(string,gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(timePicker)));
	string = expandString(string," ");
	string = expandString(string,gtk_entry_get_text(GTK_ENTRY(command)));
	string = expandString(string,"\n");
	return string;
}

GtkWidget *dragSource = NULL;

gboolean on_drag_drop (GtkWidget      *widget,
               GdkDragContext *context,
               gint            x,
               gint            y,
               guint           time,
               GtkWidget *mainTable)
{
	//printf("on_drag_drop: %s\n",gtk_widget_get_name (widget));
	if( strcmp(gtk_widget_get_name (widget),"trash") == 0 && dragSource != NULL)
	{
		gtk_drag_finish(context,TRUE,FALSE,time);
		gtk_container_remove (GTK_CONTAINER(mainTable), dragSource);
		fixLineNumbers(mainTable);
		return TRUE;
	}

	if(dragSource)
		gtk_drag_unhighlight (dragSource);
	gtk_drag_finish(context,FALSE,FALSE,time);
	dragSource = NULL;
	return TRUE;
}

void fixLineNumbers(GtkWidget *mainTable)
{
	GList *lines = gtk_container_get_children(GTK_CONTAINER(mainTable));
	int lineNumber = 0;
	for( GList *line = lines; line != NULL; line = g_list_next(line) )
	{
		lineNumber++;
		GList *firstEntry = gtk_container_get_children(GTK_CONTAINER(line->data));
		GtkLabel* label = GTK_LABEL(firstEntry->data); // first child

		int length = snprintf( NULL, 0, "%d", lineNumber );
		char* lineNumberStr = malloc( length + 1 );
		snprintf( lineNumberStr, length + 1, "%d", lineNumber );
		gtk_label_set_text(label,lineNumberStr);
		free(lineNumberStr);
	}
}

gboolean on_drag_motion(GtkWidget      *widget,
        GdkDragContext *context,
        gint            x,
        gint            y,
        guint           time,GtkWidget *mainTable)
{
	if(dragSource == NULL)
		return FALSE;

	//printf("on_drag_motion: %s\n",gtk_widget_get_name (widget));

	GtkWidget *destBox = NULL;
	if( strcmp(gtk_widget_get_name (widget),"GtkEntry") == 0)
	{
		destBox = gtk_widget_get_parent(widget);
	}
	else if( strcmp(gtk_widget_get_name (widget),"GtkBox") == 0 )
	{
		destBox = widget;
	}
	else
	{
		return FALSE;
	}

	if( dragSource == destBox )
		return FALSE;

	GValue destIndex = G_VALUE_INIT;
	g_value_init (&destIndex, G_TYPE_INT);
	gtk_container_child_get_property(GTK_CONTAINER(mainTable),destBox,"position",&destIndex);
	if( g_value_get_int(&destIndex) < 0 )
		return FALSE;
	gtk_box_reorder_child (GTK_BOX(mainTable),dragSource,g_value_get_int(&destIndex));
	fixLineNumbers(mainTable);
	return TRUE;
}


//void on_drag_data_received(GtkWidget        *widget,
//        GdkDragContext   *context,
//        gint              x,
//        gint              y,
//        GtkSelectionData *data,
//        guint             info,
//        guint             time,GtkWidget *box)
//{
//	printf("on_drag_data_received\n");
//}
//
//void on_drag_data_get(GtkWidget *widget, GdkDragContext *context,GtkWidget *box)
//{
//	printf("on_drag_data_get\n");
//}

void on_drag_end(GtkWidget *widget, GdkDragContext *context,GtkWidget *mainTable)
{
	//printf("on_drag_end: %s\n",gtk_widget_get_name (widget));
	if(dragSource)
		gtk_drag_unhighlight (dragSource);
	dragSource = NULL;
}

void on_drag_begin(GtkWidget *widget, GdkDragContext *context,GtkWidget *box)
{
	dragSource = box;
	gtk_drag_dest_unset (box); //drag-source has to differ from drag-dest
	//printf("on_drag_begin\n");
	gtk_drag_highlight (box);
	//gtk_container_remove(GTK_CONTAINER(box),widget);
	//gtk_drag_set_icon_widget ( context, widget, 1, 1);
}

void addTimeSelectorButton(GtkWidget *buttonBox, int type, const char* value )
{
	GtkWidget *button = gtk_button_new_with_label (value);
	gtk_container_add (GTK_CONTAINER (buttonBox), button);
	g_signal_connect (button, "clicked", G_CALLBACK (on_time_selector_pressed), type);
}

GtkWidget* addDragDropButton(GtkWidget *box, GtkWidget *mainTable )
{
	GtkWidget *dragButton = gtk_entry_new ();
	setDragDestination(mainTable, dragButton);
	gtk_entry_set_width_chars (GTK_ENTRY(dragButton),3);
	gtk_entry_set_icon_from_icon_name (GTK_ENTRY(dragButton), GTK_ENTRY_ICON_SECONDARY,"view-app-grid-symbolic.symbolic");
	GValue editable = G_VALUE_INIT;
	g_value_init (&editable, G_TYPE_BOOLEAN	);
	g_value_set_boolean (&editable,FALSE);
	g_object_set_property(G_OBJECT(dragButton),"editable",&editable);
	GtkTargetList * list = gtk_target_list_new (targetentries, nTargetEntries);
	gtk_entry_set_icon_drag_source (GTK_ENTRY(dragButton), GTK_ENTRY_ICON_SECONDARY, list, GDK_ACTION_MOVE);
	g_signal_connect(dragButton, "drag-begin",G_CALLBACK(on_drag_begin),box);
	g_signal_connect(dragButton, "drag-end",G_CALLBACK(on_drag_end),mainTable);
	//g_signal_connect(dragButton, "drag-data-get",G_CALLBACK(on_drag_data_get),mainTable);
	gtk_container_add (GTK_CONTAINER (box), dragButton);
	return dragButton;
}

void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_set_name (box,advancedJobFragmentName);
	gtk_container_add (GTK_CONTAINER (mainTable), box);
	setDragDestination(mainTable, box);

	addLineNumber(mainTable,box);

	GtkWidget *buttons = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (box), buttons);
	gtk_size_group_add_widget (sizeGroupTimePickerBox,buttons);

	addTimeSelectorButton(buttons,MINUTE,minute);
	addTimeSelectorButton(buttons,HOUR,hour);
	addTimeSelectorButton(buttons,DOM,dom);
	addTimeSelectorButton(buttons,MONTH,month);
	addTimeSelectorButton(buttons,DOW,dow);

	GtkWidget *commandBox = gtk_entry_new();
	setDragDestination(mainTable, commandBox);
	gtk_container_add (GTK_CONTAINER (box), commandBox);
	gtk_entry_set_text(GTK_ENTRY(commandBox),command);
	gtk_widget_set_hexpand (commandBox,TRUE);

	addDragDropButton(box,mainTable);
	gtk_widget_show_all (box);
}

char* advancedCronJobToString(GtkWidget *advancedCronJob, char* string)
{
	GList *element_lineNumber = gtk_container_get_children(GTK_CONTAINER(advancedCronJob));
	GList *element_buttons = g_list_next(element_lineNumber);
	GtkWidget* timeSelector;
	GList *element_minute = gtk_container_get_children(GTK_CONTAINER(element_buttons->data));
	timeSelector =  element_minute->data;
	string = expandString(string,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	string = expandString(string," ");
	GList *element_hour = g_list_next(element_minute);
	timeSelector =  element_hour->data;
	string = expandString(string,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	string = expandString(string," ");
	GList *element_dom = g_list_next(element_hour);
	timeSelector =  element_dom->data;
	string = expandString(string,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	string = expandString(string," ");
	GList *element_month = g_list_next(element_dom);
	timeSelector =  element_month->data;
	string = expandString(string,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	string = expandString(string," ");
	GList *element_dow = g_list_next(element_month);
	timeSelector =  element_dow->data;
	string = expandString(string,gtk_button_get_label(GTK_BUTTON(timeSelector)));
	string = expandString(string," ");

	GList *element_command = g_list_next(element_buttons);
	GtkWidget* command =  element_command->data;
	string = expandString(string,gtk_entry_get_text(GTK_ENTRY(command)));
	string = expandString(string,"\n");
	return string;
}

void on_menu_new(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: new pressed\n");
}

void on_menu_open(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: open pressed\n");
}

void on_menu_save(GtkWidget *menuItem, GtkWidget *mainTable )
{
	char* string = mainTableToString(mainTable);
	printf("%s",string);
	write_cron_tab("testfile.txt",string);
}

void on_menu_save_as(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: save as pressed\n");
}

void on_menu_close(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: close pressed\n");
}

void on_menu_copy(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: copy pressed\n");
}

void on_menu_paste(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: paste pressed\n");
}

void on_menu_add_comment(GtkWidget *menuItem, GtkWidget *mainTable )
{
	addCommentOrVariable(mainTable, "# Put comment here");
}

void on_menu_add_simple_job(GtkWidget *menuItem, GtkWidget *mainTable )
{
	addSimpleCronJob(mainTable, simpleTimingValues[0], "command to execute");
}

void on_menu_add_advanced_job(GtkWidget *menuItem, GtkWidget *mainTable )
{
	addAdvancedCronJob( mainTable, "0", "0", "*", "*", "*", "command to execute");
}

void on_menu_index(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: index pressed\n");
}

void on_menu_about(GtkWidget *menuItem, GtkWidget *mainTable )
{
	printf("TODO: about pressed\n");
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

char* mainTableToString(GtkWidget *mainTable)
{
	char* string = NULL;
	GList *lines = gtk_container_get_children(GTK_CONTAINER(mainTable));
	for( GList *line = lines; line != NULL; line = g_list_next(line) )
	{
		const char* lineType = gtk_widget_get_name(line->data);
		if(  strcmp(lineType,commentOrVariableFragmentName) == 0 )
		{
			string = commentOrVariableToString(line->data,string);
		}
		else if(  strcmp(lineType,simpleJobFragmentName) == 0 )
		{
			string = simpleCronJobToString(line->data,string);
		}
		else if(  strcmp(lineType,advancedJobFragmentName) == 0 )
		{
			string = advancedCronJobToString(line->data,string);
		}
		else
		{
			// TODO: print into status bar
			printf("Error: the widget '%s' could not be converted to string \n",lineType);
		}
	}
	return string;
}
