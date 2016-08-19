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

	printf("Here you can find the source-code for this application: https://github.com/cron-job/cron-gui\n");
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
	char* fileToLoad = parse_arguments(argc, argv);

    gtk_init(&argc, &argv);

	// load css file
//	GtkCssProvider *cssProvider = gtk_css_provider_new ();
//	gtk_css_provider_load_from_path(cssProvider,"./test.css",NULL);
//	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "main_window.glade", NULL);

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show(window);
    main_window = GTK_WINDOW(window);

    GtkWidget *mainTable = gtk_builder_get_object (builder,"mainTable");
    initSizeGroups();
    if( fileToLoad == NULL)
    	read_cron_tab(mainTable,"crontab");
    else
    	read_cron_tab(mainTable,fileToLoad);

	gtk_builder_connect_signals(builder, NULL);

    gtk_main();
	g_object_unref(builder);
    return 0;
}

