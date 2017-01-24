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

#ifndef WIZARD_H
#define WIZARD_H

#include <gtk/gtk.h>


static const char *buttonName[] = { "Select Minutes", "Select Hours", "Select Days of Month", "Select Months", "Select Day of Week" };

typedef enum { MINUTE, HOUR, DOM, MONTH, DOW } wizardType;

struct wizardContext;

struct wizard_launch_context;

// returns null on success
void runWizard( GtkWindow *main_window, GtkWidget *pressedButton);

void on_switch_page(GtkNotebook *notebook, GtkWidget *page,guint page_num, struct wizardContext *context);

void on_move_scaler(GtkRange *range, struct wizardContext *context);

void on_toggle_grid_button(GtkToggleButton *togglebutton, struct wizardContext *context);

#endif //WIZARD_H
