#ifndef GENFILES_H
#define GENFILES_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "string.h"

void genAllIncludes(){
	struct dirent *files;
	DIR *dir = opendir("../Scripts");
	remove("allIncludes.h");
	FILE *fp = fopen("allIncludes.h","a+");
	fprintf(fp,"#ifndef ALLINCLUDES_H\n#define ALLINCLUDES_H\n#include \"tfuncs.h\"\n");
	while ((files = readdir(dir)) != NULL){
		if(strcmp("..",files->d_name) && strcmp(".",files->d_name))
			fprintf(fp,"#include \"../Scripts/%s\"\n", files->d_name);
	}
	fprintf(fp,"#endif\n");
	fclose(fp);
	closedir(dir);
	free(files);
}

void genScriptFile(char *objectName){
	string_t *fstring = defineStringFromCharPtr(objectName, 0);
	appendCharPtr(".h", &fstring, 0);
	char *fileName = typeCastToCharPtr(fstring);
	freeString(fstring);
	FILE *fp = fopen(fileName,"w+");
	free(fileName);
	fprintf(fp,"#ifndef %s_H\n#define %s_H\n\n#include \"../Tools/tfuncs.h\"\n#include \"../Tools/render.h\"\n\nint %sStartYPos; // because Y doesn't start at zero, thought it might help if you knew where it does start\n\nvoid %sStartFunc(gObject_t *self){\n\t%sStartYPos = self->y;\n\t//Your code here will be ran at the start of the program\n}\n\nvoid %sLoopFunc(gObject_t *self, char keypress){\n\t//Whatever you write here will be ran every frame\n}\n#endif", objectName, objectName, objectName, objectName, objectName, objectName);
	fclose(fp);
}

void compileToOneFile(){
	struct dirent *files;
	DIR *dir = opendir("Objects");
	remove("main.c");
	FILE *fp = fopen("main.c","a+");
	fprintf(fp,"#include <ncurses.h>\n#include <stdlib.h>\n#include \"Tools/tfuncs.h\"\n#include \"Tools/render.h\"\n#include \"config.h\"\n#include \"Tools/allIncludes.h\"\nint main(){\n\tinitscr();\n\tnoecho();\n\tnodelay(stdscr, TRUE);\n\tscrollok(stdscr, TRUE);\n\tfor(int i = 0; i < SLEEPCOUNT; i++)\n\t\tsleeps[i] = 0;\n\tchar key = 0;\n\t");
	int dirSize = 0;
	while ((files = readdir(dir)) != NULL)
		if(strcmp("..",files->d_name) && strcmp(".",files->d_name))
			dirSize++;
	rewinddir(dir);
	while ((files = readdir(dir)) != NULL)
		if(strcmp("..",files->d_name) && strcmp(".",files->d_name))
			fprintf(fp,"gObject_t *%sObj = calloc(1, sizeof(gObject_t));\n\treadFromFile(\"Objects/%s\",%sObj);\n\t", files->d_name, files->d_name, files->d_name);
	rewinddir(dir);
	fprintf(fp,"char **screen = calloc(LINES, sizeof(char*));\n\tfor(int x = 0; x < LINES; x++){\n\t\tscreen[x] = calloc(COLS+1, sizeof(char));\n\t\tfor(int z = 0; z < COLS; z++)\n\t\t\tscreen[x][z] = ' ';\n\t}\n\t");
	while ((files = readdir(dir)) != NULL)
		if(strcmp("..",files->d_name) && strcmp(".",files->d_name))
			fprintf(fp,"%sStartFunc(%sObj);\n\t", files->d_name, files->d_name);
	fprintf(fp,"while(key != EXITKEY){\n\t\tbegin = clock();\n\t\tkey = getch();\n\t\t");
	rewinddir(dir);
	while ((files = readdir(dir)) != NULL)
		if(strcmp("..",files->d_name) && strcmp(".",files->d_name))
			fprintf(fp,"%sLoopFunc(%sObj,key);\n\t\trender(%sObj, screen);\n\t\t", files->d_name, files->d_name, files->d_name);
	fprintf(fp,"for(int y = 0; y < LINES; y++){move(y,0);printw(\"%s\", screen[y]);for(int x = 0; x < COLS; x++){screen[y][x] = ' ';}}\n\t\tgetDeltaTime();\n\t}\n\t","%s");
	rewinddir(dir);
	while ((files = readdir(dir)) != NULL)
		if(strcmp("..",files->d_name) && strcmp(".",files->d_name))
			fprintf(fp,"free(%sObj);\n\t", files->d_name);
	fprintf(fp,"for(int y = 0; y < LINES; y++){free(screen[y]);}\n\tfree(screen);\n\tendwin();\nreturn 0;\n}");
	fclose(fp);
	closedir(dir);
	free(files);
}

#endif
