#include "wizard.h"
#include "readCrontab.h"

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
						{ 1, 30, 1 }, //minute
						{ 1, 12, 1 }, //hour
						{ 1, 15, 1 }, //dom
						{ 1,  6, 1 }, //month
						{ 1,  3, 1 }  //dow
					  };

int buttonMatrix [][5] = { // rows, columns, entries
						{ 6, 10, 60 }, //minute
						{ 4,  6, 24 }, //hour
						{ 5,  7, 31 }, //dom
						{ 3,  4, 12 }, //month
						{ 1,  7, 7 }  //dow
					  };

struct wizardContext
{
	GtkEntry *timeRule;
	GtkWidget *scaler;
	GtkGrid *grid;
	wizardType type;
};

void runWizard( wizardType type, GtkWindow *main_window, GtkWidget *pressedButton )
{
	struct wizardContext context;
	context.type = type;

	GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget *wizard = gtk_dialog_new_with_buttons (windowTitle[type],main_window,flags,"_OK", GTK_RESPONSE_ACCEPT, "_Cancel", GTK_RESPONSE_REJECT, NULL);
	gtk_window_set_transient_for (GTK_WINDOW(wizard),main_window);
	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (wizard));

	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
	gtk_widget_set_vexpand (content_area,TRUE);

	gtk_widget_set_vexpand (box,TRUE);

	gtk_container_add (GTK_CONTAINER (content_area), box);

	context.timeRule =  gtk_entry_new();
	gtk_container_add (GTK_CONTAINER (box), context.timeRule);

	context.scaler = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, scalerLimits[type][0],scalerLimits[type][1],scalerLimits[type][2]);
	g_signal_connect (context.scaler, "value-changed", G_CALLBACK(on_move_scaler), &context);

	context.grid = GTK_GRID(gtk_grid_new());
	gtk_grid_set_row_homogeneous (context.grid,TRUE);
	gtk_grid_set_column_homogeneous (context.grid,TRUE);
	int nRows = buttonMatrix[type][0];
	int nColumns = buttonMatrix[type][1];
	int nElements = buttonMatrix[type][2];
	for( int i= 0; i < nRows; i++ )
		gtk_grid_insert_row (context.grid,0);
	for( int j= 0; j < nColumns; j++ )
		gtk_grid_insert_column (context.grid,0);

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
					toggleButton = gtk_toggle_button_new_with_label (DowNames[currentElement + 1]); // +1, because for compartibilty cron keeps sundays twice ( we want to start at monday)
				else
				{
					int length = snprintf( NULL, 0, "%d", currentElement + 1 );
					char* temp = malloc( length + 1 );// one more for 0-terminator
					snprintf( temp, length + 1, "%d", currentElement + 1 );
					toggleButton = gtk_toggle_button_new_with_label (temp);
					free(temp);
				}
				gtk_grid_attach (GTK_GRID(context.grid),toggleButton,j,i,1,1);
				g_signal_connect (toggleButton, "toggled", G_CALLBACK(on_toggle_grid_button), &context);
				currentElement++;
			}
		}
	}

	GtkWidget *notebook = gtk_notebook_new();
	gtk_widget_set_vexpand (notebook,TRUE);
	gtk_container_add (GTK_CONTAINER (box), notebook);
	GtkWidget * label = gtk_label_new("");
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook),label,gtk_label_new (tabNames[type][0]));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook),context.scaler,gtk_label_new (tabNames[type][1]));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook),GTK_WIDGET(context.grid),gtk_label_new (tabNames[type][2]));
	g_signal_connect (notebook, "switch-page", G_CALLBACK(on_switch_page), &context);
	gtk_widget_show_all (wizard);
	gint result = gtk_dialog_run (GTK_DIALOG (wizard));
	if( result == GTK_RESPONSE_ACCEPT)
		gtk_button_set_label(GTK_BUTTON(pressedButton), gtk_entry_get_text (GTK_ENTRY(context.timeRule)));

	gtk_widget_destroy (wizard);
}

void setTimeRuleFromScaler(struct wizardContext *context)
{
	gdouble value = gtk_range_get_value (GTK_RANGE(context->scaler));
	int valueInt = value; // only full digits allowed in GUI
	if( valueInt == 1 )
	{
		gtk_entry_set_text (GTK_ENTRY(context->timeRule),"*");
		return;
	}
	int length = snprintf( NULL, 0, "*/%d", valueInt );
	char* temp = malloc( length + 1 );// one more for 0-terminator
	snprintf( temp, length + 1, "*/%d", valueInt );
	gtk_entry_set_text (GTK_ENTRY(context->timeRule),temp);
	free(temp);
}

void setTimeRuleFromButtonGrid(struct wizardContext *context)
{
	GList *buttons = gtk_container_get_children(GTK_CONTAINER(context->grid));
	unsigned int numberOfGridElements = g_list_length(buttons);
	bitstr_t bit_decl(bitstring, numberOfGridElements);

	int index = numberOfGridElements; // 1 ... N

	while ( buttons != NULL && index > 0)
	{
		int bitToUse;
		if ( context->type == MINUTE || context->type == HOUR || context->type == DOW )
		{
			bitToUse = index;
			if( index == numberOfGridElements )
				bitToUse = 0;
		}
		else
		{
			bitToUse = index - 1;
		}
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttons->data)))
			bit_set(bitstring, bitToUse);
		else
			bit_clear(bitstring, bitToUse);
		buttons = g_list_next(buttons);
		index--;
	}
	char string[MAX_COMMAND] = "\0";
	switch( context->type )
	{
		case DOW:
			bitStrToString(bitstring, string, numberOfGridElements,DowNames);
			break;
		case MONTH:
			bitStrToString(bitstring, string, numberOfGridElements,MonthNames);
			break;
		default:
			bitStrToString(bitstring, string, numberOfGridElements,NULL);
	}
	gtk_entry_set_text (GTK_ENTRY(context->timeRule),string);
}

void on_move_scaler(GtkRange *range, struct wizardContext *context)
{
	setTimeRuleFromScaler(context);
}

void on_toggle_grid_button(GtkToggleButton *togglebutton, struct wizardContext *context)
{
	setTimeRuleFromButtonGrid(context);
}

void on_switch_page(GtkNotebook *notebook, GtkWidget *page,guint page_num, struct wizardContext *context)
{
	if( page_num == PAGE_1_EVERY_X )
		gtk_entry_set_text (GTK_ENTRY(context->timeRule),"*");
	else if( page_num == PAGE_2_EVERY_N_X )
	{
		setTimeRuleFromScaler(context);
	}
	else if( page_num == PAGE_3_EACH_SELECTED_X )
	{
		setTimeRuleFromButtonGrid(context);
	}
}
