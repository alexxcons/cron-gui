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

enum
{
  TARGET_WHATEVER,
};

static GtkTargetEntry targetentries[] =
{
  { "GtkBox",        0, TARGET_WHATEVER },
  { "STRING",        0, TARGET_WHATEVER },
  { "ICON",        0, TARGET_WHATEVER },
};

const gint nTargetEntries = 3;

void setDragDestination(GtkWidget *mainTable, GtkWidget *line)
{
gtk_drag_dest_set (line,
		GTK_DEST_DEFAULT_MOTION,
		targetentries,
		nTargetEntries,
                   GDK_ACTION_MOVE);
//g_signal_connect(line, "drag-data-received",G_CALLBACK(on_drag_data_received),line);
g_signal_connect(line, "drag-motion",G_CALLBACK(on_drag_motion),mainTable);
}

void addCommentOrVariable(GtkWidget *mainTable, const char *text)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	setDragDestination(mainTable, box);
	gtk_container_add (GTK_CONTAINER (mainTable), box);

	addLineNumber(box);

	GtkWidget *textbox = gtk_entry_new ();
	setDragDestination(mainTable, textbox);
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
	setDragDestination(mainTable, box);

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
	setDragDestination(mainTable, commandBox);
	gtk_entry_set_text(commandBox,command);
	gtk_widget_set_hexpand (commandBox,TRUE);
	gtk_widget_show_all (box);
}

void addTimeSelectorButton(GtkWidget *buttonBox, int type, const char* value )
{
	GtkWidget *button = gtk_button_new_with_label (value);

	struct GtkTargetEntry {
	  gchar *target;
	  guint  flags;
	  guint  info;
	};

	gtk_container_add (GTK_CONTAINER (buttonBox), button);
	g_signal_connect (button, "clicked", G_CALLBACK (on_time_selector_pressed), type);
}

GtkWidget *dragSource = NULL;

gboolean on_drag_motion(GtkWidget      *widget,
        GdkDragContext *context,
        gint            x,
        gint            y,
        guint           time,GtkWidget *mainTable)
{
	if(dragSource == NULL)
		return FALSE;

	printf("on_drag_motion: %s\n",gtk_widget_get_name (widget));

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

	GValue destIndex = G_VALUE_INIT;
	g_value_init (&destIndex, G_TYPE_INT);
	gtk_container_child_get_property(mainTable,destBox,"position",&destIndex);
	if( g_value_get_int(&destIndex) < 0 )
		return FALSE;

	printf("wiget %d\n",g_value_get_int(&destIndex));
	gtk_box_reorder_child (mainTable,dragSource,g_value_get_int(&destIndex));
	return TRUE;
}


void on_drag_data_received(GtkWidget        *widget,
        GdkDragContext   *context,
        gint              x,
        gint              y,
        GtkSelectionData *data,
        guint             info,
        guint             time,GtkWidget *box)
{
	printf("on_drag_data_received\n");
}

void on_drag_data_get(GtkWidget *widget, GdkDragContext *context,GtkWidget *box)
{
	printf("on_drag_data_get\n");
}

void on_drag_end(GtkWidget *widget, GdkDragContext *context,GtkWidget *box)
{
	dragSource = NULL;
	printf("on_drag_end\n");
	//GtkWidget *parent = gtk_widget_get_parent(widget);
	//gtk_container_remove(GTK_CONTAINER(parent),widget);
	//gtk_container_add(GTK_CONTAINER(box),widget);
	//gtk_drag_unhighlight (box);
}

void on_drag_begin(GtkWidget *widget, GdkDragContext *context,GtkWidget *box)
{
	dragSource = box;
	gtk_drag_dest_unset (box); //drag-source has to differ from drag-dest
	printf("on_drag_begin\n");
	//gtk_drag_highlight (box);
	//gtk_container_remove(GTK_CONTAINER(box),widget);
	//gtk_drag_set_icon_widget ( context, widget, 1, 1);
}

void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command)
{
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (mainTable), box);
	setDragDestination(mainTable, box);

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
	setDragDestination(mainTable, commandBox);
	gtk_container_add (GTK_CONTAINER (box), commandBox);
	gtk_entry_set_text(commandBox,command);
	gtk_widget_set_hexpand (commandBox,TRUE);

	GtkWidget *dragButton = gtk_entry_new ();
	gtk_entry_set_width_chars (dragButton,3);
	gtk_entry_set_icon_from_icon_name (dragButton, GTK_ENTRY_ICON_SECONDARY,"view-app-grid-symbolic.symbolic");
	GValue editable = G_VALUE_INIT;
	g_value_init (&editable, G_TYPE_BOOLEAN	);
	g_value_set_boolean (&editable,FALSE);
	g_object_set_property(dragButton,"editable",&editable);
	GtkTargetList * list = gtk_target_list_new (targetentries, nTargetEntries);
	gtk_entry_set_icon_drag_source (dragButton, GTK_ENTRY_ICON_SECONDARY, list, GDK_ACTION_MOVE);
	g_signal_connect(dragButton, "drag-begin",G_CALLBACK(on_drag_begin),box);
	g_signal_connect(dragButton, "drag-end",G_CALLBACK(on_drag_end),box);
	g_signal_connect(dragButton, "drag-data-get",G_CALLBACK(on_drag_data_get),box);
	gtk_container_add (GTK_CONTAINER (box), dragButton);

	gtk_widget_show_all (box);
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

void on_main_window_destroy()
{
    gtk_main_quit();
}

void on_time_selector_pressed(GtkWidget *button, wizardType type)
{
	runWizard( type, main_window, button);
	//printf("data: %i",data);
}
