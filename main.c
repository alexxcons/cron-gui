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

#include <gtk/gtk.h>
#include <readCrontab.h>
#include <cron_gui.h>
int verboseMode; // global defined in readCrontab.h

static void print_help(char* progName)
{
	printf("Usage: %s \n",progName);
	printf("or:\t%s -l FILE\n",progName);
	printf("or:\t%s [OPTION] ... -l FILE\n\n",progName);

	printf("Launches a graphical editor in order to ease modifications in crontab files.\n");
	printf("\t-h: help - Prints this help message\n");
	printf("\t-v: verbose - Prints additional console output\n");
	printf("\t-l: load - Loads the passed crontab-file\n\n");

	printf("AUTHOR\n");
	printf("\tWritteb by Alexander Schwinn.\n\n");

	printf("REPORT ERRORS\n");
	printf("\tPlease use the bug report tools of your operating system to report bugs on this software\n\n");

	printf("COPYRIGHT\n");
	printf("\tCopyright  ©  2016 Alexander Schwinn Lizenz GPLv3+: GNU GPL Version 3 oder neuer <http://gnu.org/licenses/gpl.html>.\n");
	printf("\tDies ist freie Software: Sie können sie verändern und weitergeben. Es gibt KEINE GARANTIE, soweit gesetzlich zulässig.\n");
}

static char* parse_arguments(int argc, char	*argv[])
{
	int	argch;
	while (EOF != (argch = getopt(argc, argv, "vhl:")))
	{
		switch (argch)
		{
			case 'v': //verbose
				verboseMode = TRUE;
				return NULL;
			case 'l': // load file
				return optarg;
			case 'h': // print help
				print_help(argv[0]);
				exit(0);
			default:
				print_help(argv[0]);
				exit(1);
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	const char* fileToLoad = parse_arguments(argc, argv);

	GtkApplication *app = gtk_application_new ("org.gtk.cron-gui", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate_main_gui), fileToLoad);
	int status = g_application_run (G_APPLICATION (app), 0, NULL);
	g_object_unref (app);
	return status;
}

