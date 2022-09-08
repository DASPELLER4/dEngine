#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "string.h"
#include "split.h"
#include "../config.h"

#if defined(USE_VIM)
	#define EDITOR "vim "
#elif defined(USE_NANO)
	#define EDITOR "nano "
#else
	#define EDITOR "cat > "
#endif

void execute(split_t* args){
	// HUGE IF STATEMENT FOR ALL COMMANDS
	static char projectFolder[512] = {0};
	if(strcmp(args->splits[0],"help") == 0){
		printf("HELP PAGE\nhelp - shows this page\nquit - quits the program\nproject - opens/creates project folder\nscript - edits existing script file (see object command to create script)\n");
	} else if(strcmp(args->splits[0],"project") == 0){
		if(args->length == 2){
			struct stat check;
			if (stat(args->splits[1], &check) == 0 && S_ISDIR(check.st_mode)) {
				printf("Environment Set to %s\n", args->splits[1]);
				strcpy(projectFolder, args->splits[1]);
			} else {
				if(!mkdir(args->splits[1],0777)){
					chdir(args->splits[1]);
					mkdir("Scripts",0777);
					mkdir("Objects",0777);
					mkdir("Tools",0777);
					chdir("../");
					printf("Created folder\nEnvironment Set to %s\n", args->splits[1]);
					strcpy(projectFolder, args->splits[1]);
				} else {
					printf("Failed to create project folder\n");
				}
			}
		} else {
			printf("Argument [project name] expected\n");
		}
	} else if(strcmp(args->splits[0],"script") == 0){
		if(!projectFolder[0]){
			printf("Project not opened, use the project command to open/create a project\n");
		} else if(args->length == 2){
			chdir(projectFolder);
			chdir("Scripts");
			string_t *fileName = defineStringFromCharPtr(args->splits[1], 0);
			appendCharPtr(".h", &fileName, 0);
			char *fName = typeCastToCharPtr(fileName);
			printf("%s\n", fName);
			if(access(fName, F_OK) == 0){
				string_t *command = defineStringFromCharPtr(EDITOR, 0);
				appendCharPtr(args->splits[1], &command, 0);
				appendCharPtr(".h", &command, 0);
				char *charCommand = typeCastToCharPtr(command);
				system(charCommand);
				free(charCommand);
				freeString(command);
			} else {
				printf("Object does not exist or corresponding script file is missing\n");
			}
			freeString(fileName);
			free(fName);
			chdir("../..");
		} else {
			printf("Argument [object name] expected\n");
		}
	} else if(strcmp(args->splits[0],"quit") == 0){
	} else {
		printf("Command not recognized - run help to see all available commands\n");
	}
}

#endif
