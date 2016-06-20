#ifndef READ_CRONTAB_H
#define READ_CRONTAB_H

#include "pkg-cron/cron.h"

void check_error(msg);
void bitStrToString(bitstr_t* bitstr, char* string, unsigned int max,char *labels[]);
void rangeToString(char range[60][4] , char* string, unsigned int* rangeIndex, int* commaNeededOnNextRange);
int read_cron_tab();

#endif //READ_CRONTAB_H
