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

#ifndef CRON_GUI_H
#define CRON_GUI_H

#include <gtk/gtk.h>
#include "wizard.h"

GtkWindow *main_window;

GtkWidget* addDragDropButton(GtkWidget *box, GtkWidget *mainTable );
void addSimpleCronJob(GtkWidget *mainTable, const char *simpleSelector, const char *command);
void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command);
void addCommentOrVariable(GtkWidget *mainTable, const char *text);

void initSizeGroups();

void activate_main_gui(GtkApplication *app, const char* fileToLoad );

// called when window is closed
void on_main_window_destroy();

void on_time_selector_pressed(GtkWidget *button, wizardType type);

void on_menu_add_comment(GtkWidget *menuItem, GtkWidget *mainTable );
void on_menu_add_simple_job(GtkWidget *menuItem, GtkWidget *mainTable );
void on_menu_add_advanced_job(GtkWidget *menuItem, GtkWidget *mainTable );

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
#endif //CRON_GUI_H
