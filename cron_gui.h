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
#include <wizard.h>
#include <config_gui_base.h>

void addSimpleCronJob(const char *simpleSelector, const char *command, context_base* context);
void addAdvancedCronJob(const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command, context_base* context);
void addCommentOrVariable(const char *text, context_base* context);

void initSizeGroups();

void activate_main_gui(GtkApplication *app, context_base* context );

void on_time_selector_pressed(GtkWidget *button, context_base* context);

void on_add_comment(GtkWidget *source, context_base* context );
void on_add_simple_job(GtkWidget *source, context_base* context );
void on_add_advanced_job(GtkWidget *source, context_base* context );

void extended2plain(void* context);
void plain2extended(void* context);


#endif //CRON_GUI_H
