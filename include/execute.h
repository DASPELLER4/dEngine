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
#include "polyEditor.h"
#include "render.h"

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
		printf("HELP PAGE\nhelp - shows this page\nquit - quits the program\nproject - opens/creates project folder\nscript - edits existing script file (see object command to create script)\nlist - lists things (use list help for more info)\nobject - creates/edits object & script and opens a gui to create it\npreview - previews object in text form\n");
	} else if(strcmp(args->splits[0],"project") == 0){
		if(args->length >= 2){
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
		} else if(args->length >= 2){
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
	} else if(strcmp(args->splits[0],"list") == 0){
		if(!projectFolder[0]){
			printf("Project not opened, use the project command to open/create a project\n");
		} else if(args->length >= 2){
			if(strcmp(args->splits[1],"help") == 0){
				printf("List help\nOptions:\nall - list all files\nscripts - list all user scripts\nobjects - list all objects\n");
			} else if(strcmp(args->splits[1],"all") == 0){
				chdir(projectFolder);
				printf("Scripts:\n");
				system("ls Scripts | paste -d '' - /dev/null");
				printf("Objects:\n");
				system("ls Objects | paste -d '' - /dev/null");
				printf("Tools:\n");
				system("ls Tools | paste -d '' - /dev/null");
				chdir("../");
			} else if(strcmp(args->splits[1],"scripts") == 0){
				chdir(projectFolder);
				printf("Scripts:\n");
				system("ls Scripts | paste -d '' - /dev/null");
				chdir("../");
			} else if(strcmp(args->splits[1],"objects") == 0){
				chdir(projectFolder);
				printf("Objects:\n");
				system("ls Objects | paste -d '' - /dev/null");
				chdir("../");
			} else
				printf("Unknown argument to command list: %s\nUse list help to list possible arguments\n", args->splits[1]);
		} else
			printf("Argument expected - use list help to see all available options\n");
	} else if(strcmp(args->splits[0],"object") == 0){
		if(!projectFolder[0]){
			printf("Project not opened, use the project command to open/create a project\n");
		} else if(args->length >= 3){
			int valid = 1;
			for(int i = 0; args->splits[1][i]; i++){
				if(args->splits[1][i] > 'z' || args->splits[1][i] < 'a')
					valid = 0;
			}
			if(valid){
				chdir(projectFolder);
				chdir("Objects");
				int brightness = atoi(args->splits[2]);
				if(brightness > 5 || brightness < 1){
					printf("Brightness has to be 1-5\n");
				} else {
					eObject_t *outputObject = polyEditor(brightness);
					FILE *fp = fopen(args->splits[1], "w");
					fprintf(fp, "");
					fclose(fp);
					fp = fopen(args->splits[1], "a+");
					int lowestX, lowestY;
					lowestX = OBJECTSCALE;
					lowestY = OBJECTSCALE;
					for(int i = 0; i<outputObject->count; i++){
						for(int j = 0; j<3; j++){
							if(gobject->polygons[i].v[j].a < lowestX)
								lowestX = gobject->polygons[i].v[j].a;
							if(gobject->polygons[i].v[j].b < lowestY)
								lowestY = gobject->polygons[i].v[j].b;
						}
					}
					for(int i = 0; i<outputObject->count; i++){
						fprintf(fp, "%d %d %d %d %d %d %c\n", (int)(gobject->polygons[i].v[0].a-lowestX), (int)(gobject->polygons[i].v[0].b-lowestY), (int)(gobject->polygons[i].v[1].a-lowestX), (int)(gobject->polygons[i].v[1].b-lowestY), (int)(gobject->polygons[i].v[2].a-lowestX), (int)(gobject->polygons[i].v[2].b-lowestY), gobject->brightness-1);
					}
					fclose(fp);
					chdir("../Scripts");
					string_t *scriptName = defineStringFromCharPtr(args->splits[1], 0);
					appendCharPtr(".h", &scriptName, 0);
					char *charScriptName = typeCastToCharPtr(scriptName);
					fp = fopen(charScriptName,"w");
					fclose(fp);
					freeString(scriptName);
					free(charScriptName);
					free(outputObject);
				}
				chdir("../..");
			} else{
				printf("Name of object cannot include anything other than lowercase alphabetical characters\n");
			}
		} else
			printf("Arguments expected [object name, object brightness]\n");
	} else if(strcmp(args->splits[0],"preview") == 0){
		if(!projectFolder[0]){
			printf("Project not opened, use the project command to open/create a project\n");
		} else if(args->length >= 2){
			chdir(projectFolder);
			chdir("Objects");
			if(access(args->splits[1], F_OK) == 0){
				renderObjectPreview(args->splits[1]);
			} else {
				printf("Object doesn't exist\n");
			}
			chdir("../..");
		} else {
			printf("Argument expected [object name]\n");
		}
	} else if(strcmp(args->splits[0],"quit") == 0){
	} else {
		printf("Command not recognized - run help to see all available commands\n");
	}
}

#endif
