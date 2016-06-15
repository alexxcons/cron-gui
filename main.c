#include <gtk/gtk.h>
#include <stdio.h>
#include "pkg-cron/cron.h"

static	int		CheckErrorCount = 0;
static	char		Filename[MAX_FNAME];

entry		*load_entry __P((FILE *, void (*)(),
				 struct passwd *, char **));

static void
check_error(msg)
	char	*msg;
{
	CheckErrorCount++;
	fprintf(stderr, "\"%s\":%d: %s\n", Filename, LineNumber-1, msg);
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
				rangeToString(&range,string,&rangeIndex,&commaNeededOnNextRange);
			}
		}
	}
	if(rangeIndex > 0)
	{
		rangeToString(&range,string,&rangeIndex,&commaNeededOnNextRange);
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

static int
read_cron_tab()
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
				//- support for @monthly etc.
				//- support for *
				//- unit-tests

				printf("debug1\n" );
				//dowToString(*(e->dow));

				char minute[200];
				minute[0] = '\0';
				bitStrToString(e->minute,minute,MINUTE_COUNT,NULL);
				printf("minutes: %s\n",minute );

				char dow[200];
				dow[0] = '\0';
				bitStrToString(e->dow,dow, DOW_COUNT-1, DowNames);// -1, because for compartibilty cron keeps sundays twice ( element #7 )
				printf("dow: %s\n",dow );

				char month[200];
				month[0] = '\0';
				bitStrToString(e->month,month,MONTH_COUNT,MonthNames);
				printf("months: %s\n",month );

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
		fprintf(stderr, "errors in crontab file, can't install.\n");
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
    GtkBuilder      *builder;
    GtkWidget       *window;

    read_cron_tab();

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main_window.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

// called when window is closed
void on_main_window_destroy()
{
	printf("debug");
    gtk_main_quit();
}


//static void
//check_error(char* msg)
//{
//	CheckErrorCount++;
//	fprintf(stderr, "\"%s\":%d: %s\n", Filename, LineNumber-1, msg);
//}


