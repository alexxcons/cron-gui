#ifndef READ_CRONTAB_H
#define READ_CRONTAB_H

#include "pkg-cron/cron.h"
#include <gtk/gtk.h>

#define	MAX_AT_STRING	10	/* Actually the maximum is 9 characters: @monthly  */

void check_error(	char	*msg);

void minuteOfEntryToString(entry *e, char* string);
void hourOfEntryToString(entry *e, char* string);
void domOfEntryToString(entry *e, char* string);
void monthOfEntryToString(entry *e, char* string);
void dowOfEntryToString(entry *e, char* string);
void commandOfEntryToString(entry *e, char* string);

void entryToString(entry *e, char* string);
void bitStrToString(bitstr_t* bitstr, char* string, unsigned int max,char *labels[]);
void rangeToString(char range[60][4] , char* string, unsigned int* rangeIndex, int* commaNeededOnNextRange);

int read_cron_tab(GtkWidget *mainTable);

#endif //READ_CRONTAB_H
