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

#ifndef READ_CRONTAB_H
#define READ_CRONTAB_H

#include "pkg-cron/cron.h"
#include <gtk/gtk.h>

#define	MAX_AT_STRING	10	/* Actually the maximum is 9 characters: @monthly  */
extern int verboseMode;

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

int read_cron_tab(GtkWidget *mainTable,char* fileToLoad);

#endif //READ_CRONTAB_H
