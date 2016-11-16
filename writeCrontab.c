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
#include <writeCrontab.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int verboseMode; // global defined in readCrontab.h

int write_cron_tab(char* fileToWrite,char* crontabString)
{
	if( verboseMode )
	{
		printf("Attempt to open file to write:\n");
		printf("%s\n",fileToWrite);
	}
	FILE* crontab  = fopen(fileToWrite,"w+");
	if (!crontab)
	{
		printf("Failed to open file '%s' : %s\n", fileToWrite, strerror(errno));
		return (-2);
	}
	fprintf(crontab, "%s\n", crontabString);
	fclose(crontab);
	return 0;
}
