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
#include "genFiles.h"

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
		printf("HELP PAGE\nhelp - shows this page\nquit - quits the program\nproject - opens/creates project folder\nscript - edits existing script file (see object command to create script)\nlist - lists things (use list help for more info)\nobject - creates/edits object & script and opens a gui to create it\npreview - previews object in text form\ncompile - compiles the program\n");
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
					struct winsize w;
					ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
					lowestX = OBJECTSCALE*MONOSPACEHEIGHTVWIDTH;
					lowestY = OBJECTSCALE;
					for(int i = 0; i<outputObject->count; i++){
						for(int j = 0; j<3; j++){
							gobject->polygons[i].v[j].a *= MONOSPACEHEIGHTVWIDTH;
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
					free(outputObject);
					chdir("../Scripts");
					string_t *scriptName = defineStringFromCharPtr(args->splits[1], 0);
					appendCharPtr(".h",&scriptName,0);
					char *scriptNameChar = typeCastToCharPtr(scriptName);
					freeString(scriptName);
					if(access(scriptNameChar, F_OK ) == -1)
						genScriptFile(args->splits[1]);
					free(scriptNameChar);
					chdir("../Tools");
					genAllIncludes();
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
				chdir("../..");
				sleep(1);
			} else {
				chdir("../..");
				printf("Object doesn't exist\n");
			}
		} else {
			printf("Argument expected [object name]\n");
		}
	} else if(strcmp(args->splits[0],"compile") == 0){
		if(!projectFolder[0]){
			printf("Project not opened, use the project command to open/create a project\n");
		} else {
			chdir(projectFolder);
			chdir("Tools");
			genAllIncludes();
			FILE *fp = fopen("render.h","w+");
			fprintf(fp, "#ifndef RENDER_H\n#define RENDER_H\n\n#include <sys/ioctl.h>\n#include <stdio.h>\n#include <unistd.h>\n#include <string.h>\n#include <stdlib.h>\n#include <ncurses.h>\n\n#include \"../config.h\"\n\ntypedef struct gVector2i{\n\tint a;\n\tint b;\n} gVector2i_t;\n\ntypedef struct gpolygon{\n\tgVector2i_t v[3];\n\tint lboundx;\n\tint lboundy;\n\tint uboundx;\n\tint uboundy;\n} gPolygon_t;\n\ntypedef struct gobject{\n\tint count;\n\tchar color;\n\tint x;\n\tint y;\n\tgPolygon_t polygons[MAXPOLYGONS];\n} gObject_t;\n\nint readFromFile(char *fileName, gObject_t *dest){\n\tFILE *fp;\n\tchar *line = NULL;\n\tsize_t lineLength = 0;\n\tssize_t read;\n\tfp = fopen(fileName, \"r\");\n\tint currPoly = 0;\n\twhile ((read = getline(&line, &lineLength, fp)) != -1){\n\t\tif(currPoly >= MAXPOLYGONS-1){\n\t\t\tprintf(\"This object has too many polygons, to support larger object complexity\\nIncrease the MAXPOLYGONS macro in config.h and re-run make\\n\");\n\t\t\tfree(line);\n\t\t\treturn 0;\n\t\t}\n\t\tchar *token = strtok(line, \" \");\n\t\tint currWord = 1;\n\t\tint currVert = 0;\n\t\twhile(token){\n\t\t\tif(currWord < 7){\n\t\t\t\tdest->polygons[currPoly].v[currVert].a = atoi(token);\n\t\t\t\tcurrWord++;\n\t\t\t\ttoken = strtok(NULL, \" \");\n\t\t\t\tdest->polygons[currPoly].v[currVert].b = atoi(token);\n\t\t\t}else{\n\t\t\t\tdest->color = token[0];\n\t\t\t}\n\t\t\tcurrWord++;\n\t\t\tcurrVert++;\n\t\t\ttoken = strtok(NULL, \" \");\n\t\t}\n\t\tdest->polygons[currPoly].lboundx = COLS;\n\t\tdest->polygons[currPoly].uboundx = 0;\n\t\tdest->polygons[currPoly].lboundy = LINES;\n\t\tdest->polygons[currPoly].uboundy = 0;\n\t\tfor(int j = 0; j<3; j++){\n\t\t\tif(dest->polygons[currPoly].v[j].a < dest->polygons[currPoly].lboundx)\n\t\t\t\tdest->polygons[currPoly].lboundx = dest->polygons[currPoly].v[j].a;\n\t\t\tif(dest->polygons[currPoly].v[j].a > dest->polygons[currPoly].uboundx)\n\t\t\t\tdest->polygons[currPoly].uboundx = dest->polygons[currPoly].v[j].a;\n\t\t\tif(dest->polygons[currPoly].v[j].b < dest->polygons[currPoly].lboundy)\n\t\t\t\tdest->polygons[currPoly].lboundy = dest->polygons[currPoly].v[j].b;\n\t\t\tif(dest->polygons[currPoly].v[j].b > dest->polygons[currPoly].uboundy)\n\t\t\t\tdest->polygons[currPoly].uboundy = dest->polygons[currPoly].v[j].b;\n\t\t}\n\t\tcurrPoly++;\n\t}\n\tfree(line);\n\tdest->count = currPoly;\n\tfclose(fp);\n\tdest->x = 0;\n\tint maxY=0;\n\tfor(int i = 0; i < dest->count; i++)\n\t\tfor(int j = 0; j < 3; j++)\n\t\t\tif(dest->polygons[i].v[j].b > maxY)\n\t\t\t\tmaxY = dest->polygons[i].v[j].b;\n\tdest->y = LINES-maxY+1;\n\treturn 1;\n}\n\nint pnpoly(gPolygon_t poly, int x, int y){ // https://github.com/eklitzke/pnpoly/blob/master/pnpoly.c\n\tint i, j, c = 0;\n\tfor(i = 0, j = 2; i < 3; j = i++)\n\t\tif (((poly.v[i].b>y) != (poly.v[j].b>y)) && (x < (poly.v[j].a-poly.v[i].a) * (y-poly.v[i].b) / (poly.v[j].b-poly.v[i].b) + poly.v[i].a))\n\t\t\tc = !c;\n\treturn c;\n}\n\nvoid render(gObject_t *object, char **screen){\n\tfor(int i = 0; i < object->count; i++){\n\t\tfor(int y = object->polygons[i].lboundy+object->y; y < object->polygons[i].uboundy+object->y; y++){\n\t\t\tfor(int x = object->polygons[i].lboundx+object->x; x < object->polygons[i].uboundx+object->x; x++){\n\t\t\t\tgPolygon_t tempPoly = (gPolygon_t){(gVector2i_t){object->polygons[i].v[0].a+object->x, object->polygons[i].v[0].b+object->y},(gVector2i_t){object->polygons[i].v[1].a+object->x, object->polygons[i].v[1].b+object->y},(gVector2i_t){object->polygons[i].v[2].a+object->x, object->polygons[i].v[2].b+object->y}, 0, 0, 0, 0};\n\t\t\t\tif(pnpoly(tempPoly, x, y)){\n\t\t\t\t\tif(!(y>=LINES || x>=COLS || x<0 || y<0))\n\t\t\t\t\t\tscreen[y][x] = object->color;\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n}\n#endif\n");
			fclose(fp);
			fp = fopen("../config.h","w+");
			fprintf(fp,"// UNCOMMENT LINE WITH YOUR EDITOR TO USE IT\n#define USE_VIM\n// #define USE_NANO\n\n// CHANGE MAXIMUM POLYGONS\n#define MAXPOLYGONS 32\n\n// DEFINE SCALE OF OBJECTS\n// #define OBJECTSCALE 50\n\n// SETS THE RATIO OF HEIGHT VS WIDTH OF A CHARACTER\n// IF AN OBJECT FILE'S X COMPONENTS ARE ONLY 0, w.ws_pixel MAY BE RETURNING 0\n// IN THAT CASE CHANGE THIS TO AROUND 1.8f\n#define MONOSPACEHEIGHTVWIDTH ((double)w.ws_xpixel/(double)w.ws_ypixel)\n\n// AMOUNT OF POSSIBLE SLEEPS\n#define SLEEPCOUNT 100\n\n// KEY TO CLOSE THE PROGRAM\n#define EXITKEY 'q'\n");
			fclose(fp);
			fp = fopen("tfuncs.h","w+");
			fprintf(fp,"#ifndef TFUNCS_H\n#define TFUNCS_H\n\n#include <time.h>\n#include \"../config.h\"\n\ndouble sleeps[SLEEPCOUNT];\ndouble deltaTime = 0.0;\n\nclock_t begin;\nclock_t end;\n\nint tsleep(int index, double time){\n\tif(sleeps[index] < time)\n\t\treturn 0;\n\telse\n\t\treturn (sleeps[index]=0)+1;\n}\n\nvoid getDeltaTime(){\n\tend = clock();\n\tdeltaTime = (double)(end - begin) / CLOCKS_PER_SEC;\n\tbegin = end;\n}\n\n#endif\n");
			fclose(fp);
			chdir("..");
			compileToOneFile();
			system("mkdir -p Completed && cp -r Objects/ Completed/Objects && gcc main.c -o Completed/game -lncurses -ltinfo && echo \"Open the project folder/Completed and run ./game to open the program\"");
			chdir("..");
		}
	} else if(strcmp(args->splits[0],"quit") == 0){
	} else {
		printf("Command not recognized - run help to see all available commands\n");
	}
}

#endif
