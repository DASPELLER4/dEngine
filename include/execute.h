#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "string.h"
#include "split.h"

void execute(split_t* args){
	// HUGE IF STATEMENT FOR ALL COMMANDS
	static char projectFolder[512] = {0};
	if(strcmp(args->splits[0],"help") == 0){
		printf("HELP PAGE\nhelp - shows this page\n");
	} else if(strcmp(args->splits[0],"project") == 0){
		if(args->length == 2){
			struct stat check;
			if (stat(args->splits[1], &check) == 0 && S_ISDIR(check.st_mode)) {
				printf("Environment Set to %s\n", args->splits[1]);
				strcpy(projectFolder, args->splits[1]);
			} else {
				if(!mkdir(args->splits[1],0777)){
					printf("Created folder\nEnvironment Set to %s\n", args->splits[1]);
					strcpy(projectFolder, args->splits[1]);
				} else {
					printf("Failed to create project folder\n");
				}
			}
		} else {
			printf("Argument [project name] expected\n");
		}
	} else if(strcmp(args->splits[0],"quit") == 0){
	} else{
		printf("Command not recognized - run help to see all available commands\n");
	}
}

#endif
