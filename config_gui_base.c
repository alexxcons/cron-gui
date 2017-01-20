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

#include <config_gui_base.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_EXIT	1

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

const int NOTEBOOK_POS_EXTENDED_EDITOR	= 0;
const int NOTEBOOK_POS_SIMPLE_EDITOR	= 1;

static GtkSizeGroup * sizeGroupLineNumbers = NULL;

GtkWidget *editMenu = NULL;

void initContext(context_base* context)
{
	context->notebook = NULL;
	context->sizeGroupLineNumbers = NULL;
	context->main_window = NULL;
	context->window = NULL;
	context->fileToLoadAtStartup = NULL;
	context->filePathCurrentlyLoaded = NULL;
	context->verboseMode = 0;
	context->cb_extended2plain = NULL;
	context->cb_plain2extended = NULL;
	context->cb_validate = NULL;
	context->gui_specific_data = NULL;
}

void displayInfo(const char* text, const char* param)
{
	printf("%s : %s\n",text,param);
	// TODO
}

void displayError(const char* text, const char* param)
{
	printf("%s : %s\n",text,param);
	// TODO
}

void markErrorInLine(GtkWidget *line)
{
	// TODO
}

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

void addLineNumber(GtkWidget *extendedEditor_linebox,GtkWidget *lineBox)
{
	GtkWidget *lineNumberLabel = gtk_label_new ("");
	gtk_container_add (GTK_CONTAINER (lineBox), lineNumberLabel);
	gtk_size_group_add_widget (sizeGroupLineNumbers,lineNumberLabel);
	fixLineNumbers(extendedEditor_linebox);
}

void setDragDestination(GtkWidget *extendedEditor_linebox, GtkWidget *widget)
{
	gtk_drag_dest_set (widget, GTK_DEST_DEFAULT_MOTION, targetentries, nTargetEntries, GDK_ACTION_MOVE);
	//g_signal_connect(widget, "drag-data-received",G_CALLBACK(on_drag_data_received),widget);
	g_signal_connect(widget, "drag-motion",G_CALLBACK(on_drag_motion),extendedEditor_linebox);
	g_signal_connect(widget, "drag-drop",G_CALLBACK(on_drag_drop),extendedEditor_linebox);
}

GtkWidget* get_extendedEditor_linebox_from_notebook(const GtkWidget* notebook)
{
	GtkWidget* extendedEditor = gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),NOTEBOOK_POS_EXTENDED_EDITOR);
	GList *extendedEditor_linebox = gtk_container_get_children(GTK_CONTAINER(extendedEditor));
	return extendedEditor_linebox->data;
}

GtkWidget* get_extendedEditor_toolbox_from_notebook(const GtkWidget* notebook)
{
	GtkWidget* extendedEditor = gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),NOTEBOOK_POS_EXTENDED_EDITOR);
	GList *extendedEditor_linebox = gtk_container_get_children(GTK_CONTAINER(extendedEditor));
	GList *extendedEditor_toolbox = g_list_next(extendedEditor_linebox);
	return extendedEditor_toolbox->data;
}

GtkWidget* get_plainTextEditor_textView_from_notebook(const GtkWidget* notebook)
{
	return gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),NOTEBOOK_POS_SIMPLE_EDITOR);
}

void plainTextEditor_textView_clear(GtkWidget* plainTextEditor_textView)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(plainTextEditor_textView));
	GtkTextIter start_iter;
	GtkTextIter end_iter;
	gtk_text_buffer_get_start_iter (buffer, &start_iter);
	gtk_text_buffer_get_end_iter (buffer, &end_iter);
	gtk_text_buffer_delete (buffer, &start_iter,&end_iter);
}

void extendedEditor_linebox_clear(GtkWidget* extendedEditor_linebox)
{
	GList* lines = gtk_container_get_children(GTK_CONTAINER(extendedEditor_linebox));
	for(GList* iter = lines; iter != NULL; iter = g_list_next(iter))
		gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(lines);
}

const char* plainTextEditor_textView_asString(GtkWidget* plainTextEditor_textView)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(plainTextEditor_textView));
	GtkTextIter start_iter;
	GtkTextIter end_iter;
	gtk_text_buffer_get_start_iter (buffer, &start_iter);
	gtk_text_buffer_get_end_iter (buffer, &end_iter);
	return gtk_text_buffer_get_text (buffer, &start_iter,&end_iter, TRUE);
}

void plainTextEditor_textView_append(GtkWidget* plainTextEditor_textView, const gchar* text)
{
	const size_t len = strlen(text);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(plainTextEditor_textView));
	GtkTextIter end_iter;
	gtk_text_buffer_get_end_iter (buffer, &end_iter);
	gtk_text_buffer_insert (buffer,&end_iter, text, len);
}

void initSizeGroupsBase(context_base* context)
{
	context->sizeGroupLineNumbers = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
}

void activate_main_gui_base(GtkApplication *app, context_base* context)
{
	//	GtkCssProvider *cssProvider = gtk_css_provider_new ();
	//	gtk_css_provider_load_from_path(cssProvider,"./test.css",NULL);
	//	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
	printf("activate_main_gui_base 1\n");
	context->window = gtk_application_window_new(app);

	g_signal_connect (context->window, "destroy", G_CALLBACK (on_main_window_destroy), NULL);
	context->main_window = GTK_WINDOW(context->window);
	gtk_window_set_default_size (GTK_WINDOW (context->window), 200, 200);

	GtkWidget *main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add (GTK_CONTAINER (context->window), main_box);

	GtkWidget *menuBar = gtk_menu_bar_new();
	gtk_container_add (GTK_CONTAINER (main_box), menuBar);

	GtkWidget *extendedEditor = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget *extendedEditor_linebox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget *extendedEditor_toolbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,3);

	gtk_container_add (GTK_CONTAINER (extendedEditor), extendedEditor_linebox);
	gtk_container_add (GTK_CONTAINER (extendedEditor), extendedEditor_toolbox);

	context->notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK(context->notebook),GTK_POS_BOTTOM);
	gtk_container_add (GTK_CONTAINER (main_box), context->notebook);
	gtk_notebook_append_page (GTK_NOTEBOOK(context->notebook),extendedEditor,gtk_label_new ("extended"));
	GtkWidget * plainTextEditor_textView = gtk_text_view_new();
	gtk_notebook_append_page (GTK_NOTEBOOK(context->notebook),plainTextEditor_textView,gtk_label_new ("plain text"));
	g_signal_connect (context->notebook, "switch-page", G_CALLBACK(on_switch_page_main_gui), context);

	GtkWidget *fileMenu = gtk_menu_new();
	GtkWidget *fileBase = gtk_menu_item_new_with_label("File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileBase), fileMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), fileBase);
	GtkWidget *fileNew = gtk_menu_item_new_with_label("New");
	GtkWidget *fileOpen = gtk_menu_item_new_with_label("Open");
	GtkWidget *fileSave = gtk_menu_item_new_with_label("Save");
	GtkWidget *fileSaveAs = gtk_menu_item_new_with_label("Save as");
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileNew);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileOpen);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileSave);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileSaveAs);
	g_signal_connect(G_OBJECT(fileNew), "activate", G_CALLBACK(on_menu_new), context);
	g_signal_connect(G_OBJECT(fileOpen), "activate", G_CALLBACK(on_menu_open), context);
	g_signal_connect(G_OBJECT(fileSave), "activate", G_CALLBACK(on_menu_save), context);
	g_signal_connect(G_OBJECT(fileSaveAs), "activate", G_CALLBACK(on_menu_save_as), context);

	GtkWidget *editMenu = gtk_menu_new();
	GtkWidget *editBase = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(editBase), editMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), editBase);
	GtkWidget *editCopy = gtk_menu_item_new_with_label("Copy");
	GtkWidget *editPaste = gtk_menu_item_new_with_label("Paste");
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editCopy);
	gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), editPaste);

	g_signal_connect(G_OBJECT(editCopy), "activate", G_CALLBACK(on_menu_copy), extendedEditor_linebox);
	g_signal_connect(G_OBJECT(editPaste), "activate", G_CALLBACK(on_menu_paste), extendedEditor_linebox);

	GtkWidget *helpMenu = gtk_menu_new();
	GtkWidget *helpBase = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpBase), helpMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), helpBase);
	GtkWidget *helpIndex = gtk_menu_item_new_with_label("Index");
	GtkWidget *helpAbout = gtk_menu_item_new_with_label("About");
	gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), helpIndex);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), helpAbout);
	g_signal_connect(G_OBJECT(helpIndex), "activate", G_CALLBACK(on_menu_index), extendedEditor_linebox);
	g_signal_connect(G_OBJECT(helpAbout), "activate", G_CALLBACK(on_menu_about), extendedEditor_linebox);

	GtkWidget *trash = gtk_image_new_from_icon_name ("user-trash-symbolic",GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_set_name(trash,"trash");
	setDragDestination(extendedEditor_linebox,trash);
	gtk_widget_set_hexpand (trash,TRUE);
	gtk_container_add (GTK_CONTAINER (extendedEditor_toolbox), trash);

	GtkWidget *statusBar = gtk_statusbar_new();
	gtk_container_add (GTK_CONTAINER (main_box), statusBar);
}

GtkWidget *dragSource = NULL;

gboolean on_drag_drop (GtkWidget      *widget,
               GdkDragContext *context,
               gint            x,
               gint            y,
               guint           time,
               GtkWidget *extendedEditor_linebox)
{
	//printf("on_drag_drop: %s\n",gtk_widget_get_name (widget));
	if( strcmp(gtk_widget_get_name (widget),"trash") == 0 && dragSource != NULL)
	{
		gtk_drag_finish(context,TRUE,FALSE,time);
		gtk_container_remove (GTK_CONTAINER(extendedEditor_linebox), dragSource);
		fixLineNumbers(extendedEditor_linebox);
		return TRUE;
	}

	if(dragSource)
		gtk_drag_unhighlight (dragSource);
	gtk_drag_finish(context,FALSE,FALSE,time);
	dragSource = NULL;
	return TRUE;
}

void fixLineNumbers(GtkWidget *extendedEditor_linebox)
{
	GList *lines = gtk_container_get_children(GTK_CONTAINER(extendedEditor_linebox));
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
        guint           time,GtkWidget *extendedEditor_linebox)
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
	gtk_container_child_get_property(GTK_CONTAINER(extendedEditor_linebox),destBox,"position",&destIndex);
	if( g_value_get_int(&destIndex) < 0 )
		return FALSE;
	gtk_box_reorder_child (GTK_BOX(extendedEditor_linebox),dragSource,g_value_get_int(&destIndex));
	fixLineNumbers(extendedEditor_linebox);
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

void on_drag_end(GtkWidget *widget, GdkDragContext *context,GtkWidget *extendedEditor_linebox)
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

GtkWidget* addDragDropButton(GtkWidget *box, GtkWidget *extendedEditor_linebox )
{
	GtkWidget *dragButton = gtk_entry_new ();
	setDragDestination(extendedEditor_linebox, dragButton);
	gtk_entry_set_width_chars (GTK_ENTRY(dragButton),3);
	gtk_entry_set_icon_from_icon_name (GTK_ENTRY(dragButton), GTK_ENTRY_ICON_SECONDARY,"view-app-grid-symbolic.symbolic");
	GValue editable = G_VALUE_INIT;
	g_value_init (&editable, G_TYPE_BOOLEAN	);
	g_value_set_boolean (&editable,FALSE);
	g_object_set_property(G_OBJECT(dragButton),"editable",&editable);
	GtkTargetList * list = gtk_target_list_new (targetentries, nTargetEntries);
	gtk_entry_set_icon_drag_source (GTK_ENTRY(dragButton), GTK_ENTRY_ICON_SECONDARY, list, GDK_ACTION_MOVE);
	g_signal_connect(dragButton, "drag-begin",G_CALLBACK(on_drag_begin),box);
	g_signal_connect(dragButton, "drag-end",G_CALLBACK(on_drag_end),extendedEditor_linebox);
	//g_signal_connect(dragButton, "drag-data-get",G_CALLBACK(on_drag_data_get),extendedEditor_linebox);
	gtk_container_add (GTK_CONTAINER (box), dragButton);
	return dragButton;
}

void unloadFile( context_base* context)
{
	printf("unloadFile 1\n");
	extendedEditor_linebox_clear(get_extendedEditor_linebox_from_notebook(context->notebook));
	//gtk_entry_set_text (GTK_ENTRY(get_plainTextEditor_textView_from_notebook(context->notebook)),"");
	free(context->filePathCurrentlyLoaded);
	context->filePathCurrentlyLoaded = NULL;
}

void loadFile( context_base* context, const char* fileToLoad)
{
	printf("loadFile 1\n");
	if( context->filePathCurrentlyLoaded != NULL )
		unloadFile(context);

	if( fileToLoad == NULL)
	{
		printf("Empty filename passed\n");
		exit(ERROR_EXIT);
	}

	if( context->verboseMode )
	{
		printf("Attempt to open file:\n");
		printf("%s\n",fileToLoad);
	}
	FILE * file = fopen(fileToLoad, "r");
	if (!file)
	{
		fprintf(stderr, "Failed to open file '%s' : %s\n", fileToLoad, strerror(errno));
		exit (ERROR_EXIT);
	}

	printf("loadFile 2\n");

	GtkWidget* plainTextEditor_textView = get_plainTextEditor_textView_from_notebook(context->notebook);
	plainTextEditor_textView_clear(plainTextEditor_textView);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(plainTextEditor_textView));
	printf("loadFile 3\n");
	GtkTextIter end_iter;
	int	ch;
	while (EOF != (ch = getc(file)))
	{
		gchar character = ch;
		printf("%c",ch);
		gtk_text_buffer_get_end_iter (buffer, &end_iter);
		gtk_text_buffer_insert (buffer,&end_iter,&character,(gint)1);
	}
	if (fclose(file) == EOF)
	{
		perror("fclose");
		exit (ERROR_EXIT);
	}

	printf("loadFile 4\n");
	context->cb_plain2extended(context);
	printf("loadFile 5\n");
	context->filePathCurrentlyLoaded = strdup(fileToLoad);

	if( context->filePathCurrentlyLoaded == 0 )
	{
		printf("Out of memory error\n");
		exit(ERROR_EXIT);
	}

}

int saveFile(char* fileToWrite, context_base* context)
{
	if( context->verboseMode )
	{
		printf("Attempt to open file to write:\n");
		printf("%s\n",fileToWrite);
	}
	FILE* file  = fopen(fileToWrite,"w+");
	if (!file)
	{
		printf("Failed to open file '%s' : %s\n", fileToWrite, strerror(errno));
		return (-2);
	}
	if(gtk_notebook_get_current_page (GTK_NOTEBOOK(context->notebook)) == NOTEBOOK_POS_EXTENDED_EDITOR )
	{
		context->cb_extended2plain(context);
	}
	GtkWidget *textView = get_plainTextEditor_textView_from_notebook(context->notebook);
	const gchar* string = plainTextEditor_textView_asString(textView);
	fprintf(file, "%s\n", plainTextEditor_textView_asString(textView));
	fclose(file);
	context->filePathCurrentlyLoaded = strdup(fileToWrite);
	return 0;
}

void on_menu_new(GtkWidget *menuItem, context_base* context )
{
	unloadFile(context);
}

void on_menu_open(GtkWidget *menuItem, context_base* context )
{
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open file",context->main_window,action,"_Cancel",GTK_RESPONSE_CANCEL,"_Open",GTK_RESPONSE_ACCEPT,NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		loadFile(context, filename );
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

void on_menu_save(GtkWidget *menuItem, context_base* context )
{
	if( context->filePathCurrentlyLoaded == NULL )
	{
		displayError("No file loaded",""); // TODO: grayed out if no file loaded
		return;
	}

	if(!saveFile(context->filePathCurrentlyLoaded, context))
	{
		displayInfo("successfully saved file:", context->filePathCurrentlyLoaded);
	}
}

void on_menu_save_as(GtkWidget *menuItem, context_base* context )
{
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File as",context->main_window,action,"_Cancel",GTK_RESPONSE_CANCEL,"_Save",GTK_RESPONSE_ACCEPT,NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		if(!saveFile(filename, context))
		{
			displayInfo("successfully saved file:", filename);
		}
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

void on_menu_copy(GtkWidget *menuItem, GtkWidget *extendedEditor_linebox )
{
	printf("TODO: copy pressed - not yet implemented\n");
}

void on_menu_paste(GtkWidget *menuItem, GtkWidget *extendedEditor_linebox )
{
	printf("TODO: paste pressed - not yet implemented\n");
}

void on_menu_index(GtkWidget *menuItem, GtkWidget *extendedEditor_linebox )
{
	printf("TODO: index pressed\n");
}

void on_menu_about(GtkWidget *menuItem, GtkWidget *extendedEditor_linebox )
{
	printf("TODO: about pressed\n");
}

void on_main_window_destroy()
{
    gtk_main_quit();
}

void on_switch_page_main_gui(GtkNotebook *notebook, GtkWidget *page,guint page_num, context_base* context )
{
	if(page_num == 0)
		context->cb_plain2extended(context);
	else
		context->cb_extended2plain(context);
}

void extendedEditorCommentLine2Text(GtkWidget *commentLine, GtkWidget *plainTextEditor_textView)
{
	GList *element_lineNumber = gtk_container_get_children(GTK_CONTAINER(commentLine));
	GList *element_textBox = g_list_next(element_lineNumber);
	GtkWidget* textbox =  element_textBox->data;
	plainTextEditor_textView_append(plainTextEditor_textView,gtk_entry_get_text(GTK_ENTRY(textbox)));
	plainTextEditor_textView_append(plainTextEditor_textView,"\n");
}

void print_help(char* progName)
{
	printf("Usage: %s \n",progName);
	printf("or:\t%s -l FILE\n",progName);
	printf("or:\t%s [OPTION] ... -l FILE\n\n",progName);

	printf("Launches a graphical editor in order to ease modifications in crontab files.\n");
	printf("\t-h: help - Prints this help message\n");
	printf("\t-v: verbose - Prints additional console output\n");
	printf("\t-l: load - Loads the passed crontab-file\n\n");

	printf("AUTHOR\n");
	printf("\tWritteb by Alexander Schwinn.\n\n");

	printf("REPORT ERRORS\n");
	printf("\tPlease use the bug report tools of your operating system to report bugs on this software\n\n");

	printf("COPYRIGHT\n");
	printf("\tCopyright  ©  2016 Alexander Schwinn Lizenz GPLv3+: GNU GPL Version 3 oder neuer <http://gnu.org/licenses/gpl.html>.\n");
	printf("\tDies ist freie Software: Sie können sie verändern und weitergeben. Es gibt KEINE GARANTIE, soweit gesetzlich zulässig.\n");
}

void parse_arguments(int argc, char *argv[], context_base* context)
{
	int	argch;
	while (EOF != (argch = getopt(argc, argv, "vhl:")))
	{
		switch (argch)
		{
			case 'v': //verbose
				context->verboseMode = TRUE;
			case 'l': // load file
				context->fileToLoadAtStartup = optarg;
			case 'h': // print help
				print_help(argv[0]);
				exit(0);
			default:
				print_help(argv[0]);
				exit(1);
		}
	}
}

int main_base(int argc, char *argv[], const char* applicationName, cb_activate_main_gui callback)
{
	context_base context;
	initContext(&context);
	parse_arguments(argc, argv, &context);

	GtkApplication *app = gtk_application_new (applicationName, G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (callback), &context);
	int status = g_application_run (G_APPLICATION (app), 0, NULL);
	g_object_unref (app);
	return status;
}

