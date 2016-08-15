#ifndef WIZARD_H
#define WIZARD_H

#include <gtk/gtk.h>


typedef enum { MINUTE, HOUR, DOM, MONTH, DOW } wizardType;

struct wizardContext;

// returns null on success
void runWizard( wizardType type, GtkWindow *main_window, GtkWidget *pressedButton);

void on_switch_page(GtkNotebook *notebook, GtkWidget *page,guint page_num, struct wizardContext *context);

void on_move_scaler(GtkRange *range, struct wizardContext *context);

void on_toggle_grid_button(GtkToggleButton *togglebutton, struct wizardContext *context);

#endif //WIZARD_H
