#ifndef CRON_GUI_H
#define CRON_GUI_H

#include <gtk/gtk.h>

GtkWidget *main_window;

GtkWidget* openGTKWindow(GtkBuilder *builder, GtkWidget *parent, char* windowName);

void addSimpleCronJob(GtkWidget *mainTable, const char *simpleSelector, const char *command);
void addAdvancedCronJob(GtkWidget *mainTable, const char *minute, const char *hour, const char *dom, const char *month, const char *dow, const char *command);
void addComment(GtkWidget *mainTable, const char *comment);
void addVariable(GtkWidget *mainTable, const char *text);

// called when window is closed
void on_main_window_destroy();

void on_minute_pressed(GtkWidget *button);
void on_hour_pressed(GtkWidget *button);
void on_dom_pressed(GtkWidget *button);
void on_month_pressed(GtkWidget *button);
void on_dow_pressed(GtkWidget *button);

#endif //CRON_GUI_H
