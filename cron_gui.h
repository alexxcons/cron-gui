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

GtkWindow *main_window;

void addSimpleCronJob(GtkWidget *mainTable, const char *simpleSelector, const char *command);
void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command);
void addComment(GtkWidget *mainTable, const char *comment);
void addVariable(GtkWidget *mainTable, const char *text);

void initSizeGroups();

void activate_main_gui(GtkApplication *app, const char* fileToLoad );

// called when window is closed
void on_main_window_destroy();

void on_cancel_pressed(GtkWidget *button, GtkWidget *wizardWindow);

void on_insert_line_activate(GtkWidget *button);

void on_loadWizard_pressed(GtkWidget *button);
void on_minute_pressed(GtkWidget *button);
void on_hour_pressed(GtkWidget *button);
void on_dom_pressed(GtkWidget *button);
void on_month_pressed(GtkWidget *button);
void on_dow_pressed(GtkWidget *button);

#endif //CRON_GUI_H
