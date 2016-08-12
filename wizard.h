#ifndef WIZARD_H
#define WIZARD_H

#include <gtk/gtk.h>


typedef enum { MINUTE, HOUR, DOM, MONTH, DOW } wizardType;

void runWizard( wizardType type, GtkWindow *main_window);

void on_switch_page(GtkNotebook *notebook, GtkWidget *page,guint page_num, GtkEntry *intervalField);



#endif //WIZARD_H
