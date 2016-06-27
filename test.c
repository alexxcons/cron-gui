#include <readCrontab.h>

static	int		CheckErrorCount = 0;
static	char		Filename[MAX_FNAME];

entry		*load_entry __P((FILE *, void (*)(),
				 struct passwd *, char **));

void assertStrEq(char* expected ,char* got)
{
	if( strcmp(expected,got) != 0 )
	{
		printf("Test for line %d failed: \n",LineNumber );
		printf("Expected string: %s \n",expected );
		printf("Got String: %s \n",got );
		exit(1);
	}
}

int testCrontab()
{
	entry	*e;
	int eof = FALSE;
	char envstr[MAX_ENVSTR];
	char	**envp = env_init();
	static	struct passwd	pw;

	FILE * crontab = fopen("test.crontab", "r");
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
				if(LineNumber == 3) // 0-23,24,25,26,40-45,50-59 * * JAN-MAR,MAY-JUN,AUG-OCT,DEC SAT,2,SUN someCommand
				{
					char month[200]= "\0";
					bitStrToString(e->month,month,MONTH_COUNT,MonthNames);
					assertStrEq("Jan-Mar,May,Jun,Aug-Oct,Dec\0",month);

					char minute[200]= "\0";
					bitStrToString(e->minute,minute,MINUTE_COUNT,NULL);
					assertStrEq("0-26,40-45,50-59\0",minute);
				}

				if(LineNumber == 5) // * * * * * some Command
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("* * * * * some Command\0",result);
				}

				if(LineNumber == 9)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("@reboot some Command\0",result);
				}

				if(LineNumber == 10)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("@yearly some Command\0",result);
				}

				if(LineNumber == 11)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("@monthly some Command\0",result);
				}

				if(LineNumber == 12)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("@weekly some Command\0",result);
				}

				if(LineNumber == 13)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("@daily some Command\0",result);
				}

				if(LineNumber == 14)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("@hourly some Command\0",result);
				}
				if(LineNumber == 17)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("2-40/2 * * * * some Command",result);
				}
				if(LineNumber == 18)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("0-20/2,30-40/2 * * * * some Command",result);
				}
				if(LineNumber == 19)
				{
					char result[200] = "\0";
					entryToString(e, result);
					assertStrEq("*/6 */5 */4 */3 */2 some Command",result);
				}

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

int main(int argc, char *argv[])
{
	testCrontab();


	printf("All tests passed !\n");
	return 0;
}


//bitStrToString(e->minute,minute,MINUTE_COUNT,NULL);
