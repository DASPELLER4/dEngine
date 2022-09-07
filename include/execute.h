#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "split.h"

void execute(split_t* args){
	// HUGE IF STATEMENT FOR ALL COMMANDS
	if(strcmp(args->splits[0],"help") == 0){
		printf("HELP PAGE\nhelp - shows this page\n");
	}else{
		printf("Command not recognized - run help to see all available commands\n");
	}
}

#endif
