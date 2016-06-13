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
				//- support for ranges
				//- support for @monthly etc.
				//- support for *

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

void bitStrToString(bitstr_t* bitstr, char* string, unsigned int max,char *labels[])
{
	int i, commaNeeded = FALSE;
	int rangeDetected = FALSE;
	int countContinousFlags = 0;
	char temp[20];

	for( i=0; i< max; i++)
	{
		if( bit_test(bitstr,i) )
		{
			countContinousFlags++;

			if ( commaNeeded )
			{
				strcat(string,",");
			}
			if(labels)
			{
				strcat(string,labels[i]);
			}
			else
			{
				int length = snprintf( NULL, 0, "%d", i );
				char* temp = malloc( length + 1 );// one more for 0-terminator
				snprintf( temp, length + 1, "%d", i );
				strcat(string,temp);
				free(temp);
			}

			commaNeeded = TRUE;
		}
		else
		{
			countContinousFlags = 0;
			// merge temp with string here
		}
	}
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


