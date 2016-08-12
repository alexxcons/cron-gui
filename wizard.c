#include "wizard.h"
#include "pkg-cron/cron.h" // DowNames and MonthNames

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const guint PAGE_1_EVERY_X = 0;
const guint PAGE_2_EVERY_N_X = 1;
const guint PAGE_3_EACH_SELECTED_X = 2;

char *windowTitle[] = { "Select Minutes", "Select Hours", "Select Days of Month", "Select Months", "Select Day of Week" };

char *tabNames[][5] = {
						{ "every minute", "every n minutes", "each selected minute"},
						{ "every hour", "every n hours", "each selected hour"},
						{ "every day of a month", "every n days", "each selected monthday"},
						{ "every month", "every n months", "each selected month"},
						{ "every day of a week", "every n days", "each selected weekday"}
					  };
gdouble scalerLimits [][5] = { // lower, upper, increment
						{ 0, 30, 1 }, //minute
						{ 0, 12, 1 }, //hour
						{ 0, 15, 1 }, //dom
						{ 0,  6, 1 }, //month
						{ 0,  3, 1 }  //dow
					  };

int buttonMatrix [][5] = { // rows, columns, entries
						{ 6, 10, 60 }, //minute
						{ 4,  6, 24 }, //hour
						{ 5,  7, 31 }, //dom
						{ 3,  4, 12 }, //month
						{ 1,  7, 7 }  //dow
					  };

void on_action_button_presssed (GtkDialog *dialog, gint response_id, gpointer user_data)
{
	printf("Haöllo asfsaef");
}

void runWizard( wizardType type, GtkWindow *main_window )
{
	GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget *wizard = gtk_dialog_new_with_buttons (windowTitle[type],main_window,flags,"_OK", GTK_RESPONSE_ACCEPT, "_Cancel", GTK_RESPONSE_REJECT, NULL);
	gtk_window_set_transient_for (GTK_WINDOW(wizard),main_window);
	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (wizard));

	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
	gtk_widget_set_vexpand (content_area,TRUE);

	gtk_widget_set_vexpand (box,TRUE);

	gtk_container_add (GTK_CONTAINER (content_area), box);

	GtkWidget *timeRule =  gtk_entry_new();
	gtk_container_add (GTK_CONTAINER (box), timeRule);

	GtkWidget *scaler = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, scalerLimits[type][0],scalerLimits[type][1],scalerLimits[type][2]);

	GtkGrid *grid = GTK_GRID(gtk_grid_new());
	gtk_grid_set_row_homogeneous (grid,TRUE);
	gtk_grid_set_column_homogeneous (grid,TRUE);
	int nRows = buttonMatrix[type][0];
	int nColumns = buttonMatrix[type][1];
	int nElements = buttonMatrix[type][2];
	for( int i= 0; i < nRows; i++ )
		gtk_grid_insert_row (grid,0);
	for( int j= 0; j < nColumns; j++ )
		gtk_grid_insert_column (grid,0);

	int currentElement = 0;
	for( int i= 0; i < nRows; i++ )
	{
		for( int j= 0; j < nColumns; j++ )
		{
			if( nElements > currentElement)
			{
				GtkWidget *toggleButton;

				if( type == MONTH )
					toggleButton = gtk_toggle_button_new_with_label (MonthNames[currentElement]);
				else if( type == DOW )
					toggleButton = gtk_toggle_button_new_with_label (DowNames[currentElement]);
				else
				{
					int length = snprintf( NULL, 0, "%d", currentElement + 1 );
					char* temp = malloc( length + 1 );// one more for 0-terminator
					snprintf( temp, length + 1, "%d", currentElement + 1 );
					toggleButton = gtk_toggle_button_new_with_label (temp);
					free(temp);
				}
				gtk_grid_attach (GTK_GRID(grid),toggleButton,j,i,1,1);
				currentElement++;
			}
		}
	}

	GtkWidget *notebook = gtk_notebook_new();
	gtk_widget_set_vexpand (notebook,TRUE);
	gtk_container_add (GTK_CONTAINER (box), notebook);
	GtkWidget * label = gtk_label_new("");
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook),label,gtk_label_new (tabNames[type][0]));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook),scaler,gtk_label_new (tabNames[type][1]));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook),GTK_WIDGET(grid),gtk_label_new (tabNames[type][2]));
	gtk_widget_show_all (wizard);
	gint result = gtk_dialog_run (GTK_DIALOG (wizard));
	switch (result)
	  {
	    case GTK_RESPONSE_ACCEPT:
	    	printf("ok pressed");
	       break;
	    default:
	    	printf("cancel pressed");
	       break;
	  }
	gtk_widget_destroy (wizard);
}



void on_switch_page(GtkNotebook *notebook, GtkWidget *page,guint page_num,GtkEntry *intervalField)
{
	gtk_entry_set_text (GTK_ENTRY(intervalField),"*");

	if( page_num == PAGE_1_EVERY_X )
		gtk_entry_set_text (GTK_ENTRY(intervalField),"*");
	else if( page_num == PAGE_2_EVERY_N_X )
		gtk_entry_set_text (GTK_ENTRY(intervalField),""); // all items pressed
	else if( page_num == PAGE_3_EACH_SELECTED_X )
		gtk_entry_set_text (GTK_ENTRY(intervalField),"");
}
