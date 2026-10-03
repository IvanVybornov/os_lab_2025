#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main() {
    char *argv[4] = {"./seq_min_max", "2", "10", NULL};

	int pid = fork();

	if ( pid == 0 ) {
		execvp( "./seq_min_max", argv );
	}

	wait( NULL );

	printf( "Finished executing the parent process\n"
	        " - the child won't get here--you will only see this once\n" );

	return 0;
}