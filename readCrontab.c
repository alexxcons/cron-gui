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

#include <readCrontab.h>
#include <stdio.h>

static	int		CheckErrorCount = 0;
static	char		Filename[MAX_FNAME];


entry		*load_entry __P((FILE *, void (*)(),
				 struct passwd *, char **));

void check_error(char *msg)
{
	CheckErrorCount++;
	fprintf(stderr, "\"%s\":%d: %s\n", Filename, LineNumber-1, msg);
}

int appendIf_At(entry *e, char* string)
{
	if( appendIf_AtReboot(e,string))
		return TRUE;
	if( appendIf_AtYearly(e,string))
		return TRUE;
	if( appendIf_AtMonthly(e,string))
		return TRUE;
	if( appendIf_AtWeekly(e,string))
		return TRUE;
	if( appendIf_AtDaily(e,string))
		return TRUE;
	if( appendIf_AtHourly(e,string))
		return TRUE;
	return FALSE;
}

int appendIf_AtReboot(entry *e, char* string)
{
	if (e->flags & WHEN_REBOOT)
	{
		strcat(string,"@reboot");
		return TRUE;
	}
	return FALSE;
}

int allBitsSet(bitstr_t* bitstr, unsigned int max)
{
	int i;
	for( i=0; i< max; i++)
	{
		if( !bit_test(bitstr,i) )
			return FALSE;
	}
	return TRUE;
}

int onlyFirstBitIsSet(bitstr_t* bitstr, unsigned int max)
{
	if( !bit_test(bitstr,0))
		return FALSE;
	int i;
	for( i=1; i< max; i++)
	{
		if( bit_test(bitstr,i) )
			return FALSE;
	}
	return TRUE;
}

int appendIf_AtYearly(entry *e, char* string)
{
	if ( onlyFirstBitIsSet(e->minute,MINUTE_COUNT) &&
			onlyFirstBitIsSet(e->hour,HOUR_COUNT) &&
			onlyFirstBitIsSet(e->dom,DOM_COUNT) &&
			onlyFirstBitIsSet(e->month,MONTH_COUNT) &&
			allBitsSet(e->dow,DOW_COUNT) &&
			(e->flags ^ DOW_STAR) == 0)
	{
		strcat(string,"@yearly");
		return TRUE;
	}
	return FALSE;
}

int appendIf_AtMonthly(entry *e, char* string)
{
	if ( onlyFirstBitIsSet(e->minute,MINUTE_COUNT) &&
			onlyFirstBitIsSet(e->hour,HOUR_COUNT) &&
			onlyFirstBitIsSet(e->dom,DOM_COUNT) &&
			allBitsSet(e->month,MONTH_COUNT) &&
			allBitsSet(e->dow,DOW_COUNT) &&
			(e->flags ^ DOW_STAR) == 0)
	{
		strcat(string,"@monthly");
		return TRUE;
	}
	return FALSE;
}

int appendIf_AtWeekly(entry *e, char* string)
{
	if ( onlyFirstBitIsSet(e->minute,MINUTE_COUNT) &&
			onlyFirstBitIsSet(e->hour,HOUR_COUNT) &&
			allBitsSet(e->dom,DOM_COUNT) &&
			allBitsSet(e->month,MONTH_COUNT) &&
			bit_test(e->dow,0) && bit_test(e->dow,7) &&   // nasty **** in cron: sunday is always set twice
			(e->flags ^ DOM_STAR) == 0)
	{
		strcat(string,"@weekly");
		return TRUE;
	}
	return FALSE;
}

int appendIf_AtDaily(entry *e, char* string)
{
	if ( onlyFirstBitIsSet(e->minute,MINUTE_COUNT) &&
			onlyFirstBitIsSet(e->hour,HOUR_COUNT) &&
			allBitsSet(e->dom,DOM_COUNT) &&
			allBitsSet(e->month,MONTH_COUNT) &&
			allBitsSet(e->dow,DOW_COUNT) &&
			e->flags  == 0)
	{
		strcat(string,"@daily");
		return TRUE;
	}
	return FALSE;
}

int appendIf_AtHourly(entry *e, char* string)
{
	if ( onlyFirstBitIsSet(e->minute,MINUTE_COUNT) &&
			allBitsSet(e->hour,HOUR_COUNT) &&
			allBitsSet(e->dom,DOM_COUNT) &&
			allBitsSet(e->month,MONTH_COUNT) &&
			allBitsSet(e->dow,DOW_COUNT) &&
			(e->flags ^ HR_STAR) == 0)
	{
		strcat(string,"@hourly");
		return TRUE;
	}
	return FALSE;
}

void entryToString(entry *e, char* string)
{
	if(!appendIf_At(e,string))
	{
		minuteOfEntryToString(e,string);
		strcat(string," ");
		hourOfEntryToString(e,string);
		strcat(string," ");
		domOfEntryToString(e,string);
		strcat(string," ");
		monthOfEntryToString(e,string);
		strcat(string," ");
		dowOfEntryToString(e,string);
	}
	strcat(string," ");
	commandOfEntryToString(e,string);
}

void entryToFragment(entry *e, GtkWidget *mainTable)
{
	char atString[MAX_AT_STRING] = "\0";
	if(appendIf_At(e,atString))
	{
		char command[MAX_COMMAND] = "\0";
		commandOfEntryToString(e,command);
		addSimpleCronJob(mainTable,atString,command );
	}
	else
	{
		char minute[MAX_COMMAND] = "\0";
		char hour[MAX_COMMAND] = "\0";
		char dom[MAX_COMMAND] = "\0";
		char month[MAX_COMMAND] = "\0";
		char dow[MAX_COMMAND] = "\0";
		char command[MAX_COMMAND] = "\0";
		minuteOfEntryToString(e,minute);
		hourOfEntryToString(e,hour);
		domOfEntryToString(e,dom);
		monthOfEntryToString(e,month);
		dowOfEntryToString(e,dow);
		commandOfEntryToString(e,command);
		addAdvancedCronJob(mainTable,minute,hour,dom,month,dow,command);
	}

}

void minuteOfEntryToString(entry *e, char* string)
{
	if( e->flags & MIN_STAR )
	{
		if( stepsStarToString(e->minute,MINUTE_COUNT,string) == FALSE ) // only false for some silly case (bug in cron) .. stepSize > max TODO: report + fix bug in cron
			strcat(string,"*");
	}
	else
		bitStrToString(e->minute,string,MINUTE_COUNT,NULL);
}

void hourOfEntryToString(entry *e, char* string)
{
	if( e->flags & HR_STAR )
	{
		if( stepsStarToString(e->hour,HOUR_COUNT,string) == FALSE ) // TODO .. see above
			strcat(string,"*");
	}
	else
		bitStrToString(e->hour,string,HOUR_COUNT,NULL);
}

void domOfEntryToString(entry *e, char* string)
{
	if( e->flags & DOM_STAR )
	{
		if( stepsStarToString(e->dom,DOM_COUNT,string) == FALSE ) // TODO .. see above
			strcat(string,"*");
	}
	else
		bitStrToString(e->dom,string,DOM_COUNT,NULL);
}

void monthOfEntryToString(entry *e, char* string)
{
	// A bit strange, there is no month-star ... so we just test if all bits are set (or all stepwise)
	if( stepsStarToString(e->month,MONTH_COUNT,string) == FALSE )
	{
		printf("debug123:, %s",string);
		bitStrToString(e->month,string,MONTH_COUNT,MonthNames);
	}
}

void dowOfEntryToString(entry *e, char* string)
{
	if( e->flags & DOW_STAR )
	{
		if( stepsStarToString(e->dow, DOW_COUNT-1,string) == FALSE)// -1, because for compartibilty cron keeps sundays twice ( element #7 )
			strcat(string,"*"); // TODO: See above
	}
	else
		bitStrToString(e->dow,string, DOW_COUNT-1, DowNames);// -1, because for compartibilty cron keeps sundays twice ( element #7 )
}

void commandOfEntryToString(entry *e, char* string)
{
	strcat(string,e->cmd);
}

// we only check for "stepwise" notation for wildcard(star). For everything else it is considered to be more readable to just write all numbers down
int stepsStarToString(bitstr_t* bitstr, unsigned int max, char* string)
{
	if(allBitsSet(bitstr,max)) // no steps used
	{
		strcat(string,"*");
		return TRUE;
	}
	int stepSize;
	for( stepSize=2; stepSize < max ; stepSize++)
	{
		if (hasStepWide(bitstr, max,stepSize))
		{
			strcat(string,"*/");
			int length = snprintf( NULL, 0, "%d", stepSize );
			char* temp = malloc( length + 1 );// one more for 0-terminator
			snprintf( temp, length + 1, "%d", stepSize );
			strcat(string,temp);
			free(temp);
			return TRUE;
		}
	}
	return FALSE;
}


int hasStepWide(bitstr_t* bitstr, unsigned int max, unsigned int stepwide)
{// attention! Method does not check for offsets in stepwide (e.g. if steps start with bit 2)
	unsigned int i;
	for( i=0; i< max; i++)
	{
		if( i%stepwide == 0 )
		{
			if(!bit_test(bitstr,i))
				return FALSE;
		}
		else
		{
			if(bit_test(bitstr,i))
				return FALSE;
		}
	}
	return TRUE;
}

void bitStrToString(bitstr_t* bitstr, char* string, unsigned int max,char *labels[])
{
	int i, commaNeededOnNextRange = FALSE;
	char range[60][4];//max 60 because of 60 minutes
	for( i= 0; i< 60; i++)
	{
		range[i][0] = '\0';
	}

	unsigned int rangeIndex = 0;

	for( i=0; i< max; i++)
	{
		if( bit_test(bitstr,i) )
		{
			if(labels)
			{
				strcat(range[rangeIndex],labels[i]);
			}
			else
			{
				int value = i;
				if ( max == DOM_COUNT )// dom has offset 1
					value++;
				int length = snprintf( NULL, 0, "%d", value );
				char* temp = malloc( length + 1 );// one more for 0-terminator
				snprintf( temp, length + 1, "%d", value );
				strcat(range[rangeIndex],temp);
				free(temp);
			}
			rangeIndex++;
		}
		else
		{

			if(rangeIndex > 0)
			{
				rangeToString(range,string,&rangeIndex,&commaNeededOnNextRange);
			}
		}
	}
	if(rangeIndex > 0)
	{
		rangeToString(range,string,&rangeIndex,&commaNeededOnNextRange);
	}
}

void rangeToString(char range[60][4] , char* string, unsigned int* rangeIndex, int* commaNeededOnNextRange)
{
	int i;
	if(*commaNeededOnNextRange)
	{
		strcat(string,",");
	}
	if(*rangeIndex <= 2)
	{
		strcat(string,range[0]);
		range[0][0] = '\0';
		if( *rangeIndex == 2 )
		{
			strcat(string,",");
			strcat(string,range[1]);
			range[1][0] = '\0';
		}

	}
	else // more than two: it pays of to use a range
	{
		strcat(string,range[0]);
		strcat(string,"-");
		strcat(string,range[*rangeIndex-1]);
		for( i= 0; i< *rangeIndex; i++)
		{
			range[i][0] = '\0';
		}
	}
	*rangeIndex = 0;
	*commaNeededOnNextRange = TRUE;
}

int get_comment(FILE *file, char *comment, int comment_size_max)
{
	long	filepos = ftell(file);
	int isComment = FALSE;
	int	ch;
	while (EOF != (ch = get_char(file)))
	{
		if( ch == '\n' )
			break;

		if( ch == '#')
			isComment = TRUE;

		if( !isComment && ch != ' ' && ch != '\t') // stop on first non comment, non whitespace if no comment found so far
			break;

		if (comment_size_max > 1) // anything longer comment_size_max than is just cut
		{
			*comment++ = (char) ch;
			comment_size_max--;
		}
	}
	if( isComment )
	{
		if (comment_size_max > 0)
			*comment = '\0';
		return TRUE;
	}
	else
	{
		fseek(file, filepos, 0);
		return FALSE;
	}
}

void get_leading_comments(FILE *crontab, GtkWidget *mainTable)
{
	char comment[MAX_ENVSTR];
	int commentFound = TRUE;
	while( commentFound )
	{
		commentFound = get_comment(crontab,comment,MAX_ENVSTR);
		if( commentFound )
		{
			addCommentOrVariable(mainTable,comment);
			printf("%s\n",comment );
		}
	}

}

int read_cron_tab(GtkWidget *mainTable,const char* fileToLoad)
{
	entry	*e;
	int eof = FALSE;
	char envstr[MAX_ENVSTR];
	char	**envp = env_init();
	static	struct passwd	pw;
	if( verboseMode )
	{
		printf("Attempt to open file:\n");
		printf("%s\n",fileToLoad);
	}
	FILE * crontab = fopen(fileToLoad, "r");
	if (!crontab)
	{
		fprintf(stderr, "Failed to open file '%s' : %s\n", fileToLoad, strerror(errno));
		return (-2);
	}

	CheckErrorCount = 0;

	while (!CheckErrorCount && !eof)
	{
		get_leading_comments(crontab,mainTable);
		switch (load_env(envstr, crontab))
		{
			case ERR:
				eof = TRUE;
				break;
			case FALSE:
				e = load_entry(crontab, check_error, &pw, envp);
				char string[MAX_COMMAND] = "\0";
				entryToString(e,string);
				entryToFragment(e,mainTable);
				printf("Cronjob: %s\n",string );
				if (e)
					free(e);
				break;
			case TRUE:
				printf("environment var: %s\n",envstr );
				addCommentOrVariable(mainTable, envstr);
				break;
		}
	}

	if (CheckErrorCount != 0)
	{
		fprintf(stderr, "errors in crontab file, can't load.\n");
		fclose(crontab);
		return (-1);
	}


	if (fclose(crontab) == EOF) {
		perror("fclose");
		return (-2);
	}
	return (0);
}
