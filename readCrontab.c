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
			bit_test(e->dow,0) && bit_test(e->dow,7) &&   // nasty **** in cron, sunday is always set twice
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

void minuteOfEntryToString(entry *e, char* string)
{
	if( e->flags & MIN_STAR )
		strcat(string,"*");
	//TODO:
	else
		bitStrToString(e->minute,string,MINUTE_COUNT,NULL);
}

void hourOfEntryToString(entry *e, char* string)
{
	if( e->flags & HR_STAR )
		strcat(string,"*");
	else
		bitStrToString(e->hour,string,HOUR_COUNT,NULL);
}

void domOfEntryToString(entry *e, char* string)
{
	if( e->flags & DOM_STAR )
		strcat(string,"*");
	else
		bitStrToString(e->dom,string,DOM_COUNT,NULL);
}

void monthOfEntryToString(entry *e, char* string)
{
	// A bit strange, there is no month-star ... so we just test for 'JAN-DEC'
	char temp[100] = "\0";
	bitStrToString(e->month,temp,MONTH_COUNT,MonthNames);
	if( strcmp(temp,"JAN-DEC\0") != 0)
		strcat(string,"*");
	else
		strcat(string,temp);
}

void dowOfEntryToString(entry *e, char* string)
{
	if( e->flags & DOW_STAR )
		strcat(string,"*");
	else
		bitStrToString(e->dow,string, DOW_COUNT-1, DowNames);// -1, because for compartibilty cron keeps sundays twice ( element #7 )
}

void commandOfEntryToString(entry *e, char* string)
{
	strcat(string,e->cmd);
}

void bitStrToString(bitstr_t* bitstr, char* string, unsigned int max,char *labels[])
{
	int i, commaNeededOnNextRange = FALSE;
	char range[60][4];
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
				int length = snprintf( NULL, 0, "%d", i );
				char* temp = malloc( length + 1 );// one more for 0-terminator
				snprintf( temp, length + 1, "%d", i );
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

int read_cron_tab()
{
	entry	*e;
	int eof = FALSE;
	char envstr[MAX_ENVSTR];
	char	**envp = env_init();
	static	struct passwd	pw;

	FILE * crontab = fopen("crontab", "r");
	if (!crontab)
	{
		fprintf(stderr, "%s/: fopen: %s\n", "PutDirectoryHere", strerror(errno));
		return (-2);
	}

	CheckErrorCount = 0;

	while (!CheckErrorCount && !eof)
	{
		switch (load_env(envstr, crontab))
		{
			case ERR:
				eof = TRUE;
				break;
			case FALSE:
				e = load_entry(crontab, check_error, &pw, envp);

				//TODO:
				//- support for step-size "/" --> take care for * !

				printf("debug1\n" );
				char string[MAX_COMMAND + 1000] = "\0";
				entryToString(e,string);
				printf("string: %s\n",string );
				printf("debug2\n" );
				if (e)
					free(e);
				break;
			case TRUE:
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
