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

#ifndef CONFIG_GUI_BASE_H
#define CONFIG_GUI_BASE_H

#include <gtk/gtk.h>

typedef char* (*concrete_gui_callback_transform)(void* context);
typedef int (*concrete_gui_callback_validate)(const char* string,char* error);

typedef struct
{
	GtkWidget* notebook;
	GtkSizeGroup* sizeGroupLineNumbers;
	GtkWindow *main_window;
	GtkWidget *window;
	char* filePathCurrentlyLoaded;
	int verboseMode;
	concrete_gui_callback_transform		cb_extended2plain;
	concrete_gui_callback_transform		cb_plain2extended;
	concrete_gui_callback_validate		cb_validate;
	void* gui_specific_data;
} context_base;


void fixLineNumbers(GtkWidget *mainTable);

GtkWidget* addDragDropButton(GtkWidget *box, GtkWidget *mainTable );

void initSizeGroupsBase(context_base* context);

void activate_main_gui_base(GtkApplication *app, const char* fileToLoad, context_base* context);

// called when window is closed
void on_main_window_destroy();


void on_menu_new(GtkWidget *menuItem, context_base* context );
void on_menu_open(GtkWidget *menuItem, context_base* context );
void on_menu_save(GtkWidget *menuItem, context_base* context );
void on_menu_save_as(GtkWidget *menuItem, context_base* context );
void on_menu_close(GtkWidget *menuItem, GtkWidget *mainTable );
void on_menu_copy(GtkWidget *menuItem, GtkWidget *mainTable );
void on_menu_paste(GtkWidget *menuItem, GtkWidget *mainTable );
void on_menu_index(GtkWidget *menuItem, GtkWidget *mainTable );
void on_menu_about(GtkWidget *menuItem, GtkWidget *mainTable );

gboolean on_drag_drop (GtkWidget      *widget,
               GdkDragContext *context,
               gint            x,
               gint            y,
               guint           time,
               GtkWidget *mainTable);
gboolean on_drag_motion(GtkWidget      *widget,
        GdkDragContext *context,
        gint            x,
        gint            y,
        guint           time,GtkWidget *box);
void on_drag_data_received(GtkWidget        *widget,
        GdkDragContext   *context,
        gint              x,
        gint              y,
        GtkSelectionData *data,
        guint             info,
        guint             time,GtkWidget *box);
void on_drag_data_get(GtkWidget *widget, GdkDragContext *context,GtkWidget *box);

void on_drag_begin(GtkWidget *widget, GdkDragContext *context,GtkWidget *box);
void on_drag_end(GtkWidget *widget, GdkDragContext *context,GtkWidget *box);

void on_switch_page_main_gui(GtkNotebook *notebook, GtkWidget *page,guint page_num, context_base* context );

GtkWidget* get_extendedEditor_linebox_from_notebook(const GtkWidget* notebook);
GtkWidget* get_extendedEditor_toolbox_from_notebook(const GtkWidget* notebook);
GtkWidget* get_plainTextEditor_textView_from_notebook(const GtkWidget* notebook);
void plainTextEditor_textView_clear(GtkWidget* plainTextEditor_textView);
void plainTextEditor_textView_append(GtkWidget* plainTextEditor_textView, const gchar* text);

void extendedEditorCommentLine2Text(GtkWidget *commentLine, GtkWidget *plainTextEditor_textView);

#endif //CONFIG_GUI_BASE_H
