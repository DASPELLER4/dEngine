#ifndef RENDER_H
#define RENDER_H

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../config.h"

typedef struct rVector2i{
	int a;
	int b;
} rVector2i_t;

typedef struct rpolygon{
	rVector2i_t v[3];
} rPolygon_t;

typedef struct robject{
	int count;
	char color;
	rPolygon_t polygons[MAXPOLYGONS];
} rObject_t;

int readFromFile(char *fileName, rObject_t *dest){
	FILE *fp;
	char *line = NULL;
	size_t lineLength = 0;
	ssize_t read;
	fp = fopen(fileName, "r");
	int currPoly = 0;
	while ((read = getline(&line, &lineLength, fp)) != -1){
		if(currPoly >= MAXPOLYGONS-1){
			printf("This object has too many polygons, to support larger object complexity\nIncrease the MAXPOLYGONS macro in config.h and re-run make\n");
			free(line);
			return 0;
		}
		char *token = strtok(line, " ");
		int currWord = 1;
		int currVert = 0;
		while(token){
			if(currWord < 7){
				dest->polygons[currPoly].v[currVert].a = atoi(token);
				currWord++;
				token = strtok(NULL, " ");
				dest->polygons[currPoly].v[currVert].b = atoi(token);
			}else{
				dest->color = token[0];
			}
			currWord++;
			currVert++;
			token = strtok(NULL, " ");
		}
		currPoly++;
	}
	free(line);
	dest->count = currPoly;
	fclose(fp);
	return 1;
}

char **makeScreen(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	char **screen = calloc(w.ws_row, sizeof(char*));
	for(int i = 0; i<w.ws_row; i++){
		screen[i] = calloc(w.ws_col+1, sizeof(char));
		for(int j = 0; j < w.ws_col; j++)
			screen[i][j] = ' ';
	}
	return screen;
}

int pnpoly(rPolygon_t poly, int x, int y){ // https://github.com/eklitzke/pnpoly/blob/master/pnpoly.c
	int i, j, c = 0;
	for(i = 0, j = 2; i < 3; j = i++)
		if (((poly.v[i].b>y) != (poly.v[j].b>y)) && (x < (poly.v[j].a-poly.v[i].a) * (y-poly.v[i].b) / (poly.v[j].b-poly.v[i].b) + poly.v[i].a))
			c = !c;
	return c;
}

void render(rObject_t *object, char **screen){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	for(int i = 0; i < object->count; i++){
		int lboundx = w.ws_col;
		int uboundx = 0;
		int lboundy = w.ws_row;
		int uboundy = 0;
		for(int j = 0; j<3; j++){
			if(object->polygons[i].v[j].a < lboundx)
				lboundx = object->polygons[i].v[j].a;
			if(object->polygons[i].v[j].a > uboundx)
				uboundx = object->polygons[i].v[j].a;
			if(object->polygons[i].v[j].b < lboundy)
				lboundy = object->polygons[i].v[j].b;
			if(object->polygons[i].v[j].b > uboundy)
				uboundy = object->polygons[i].v[j].b;
		}
		for(int y = lboundy; y < uboundy; y++){
			for(int x = lboundx; x < uboundx; x++){
				if(pnpoly(object->polygons[i], x, y)){
					if(!(y>=w.ws_row || x>=w.ws_col))
						screen[y][x] = object->color;
				}
			}
		}
	}
}

void renderObjectPreview(char *fileName){
	rObject_t *object = calloc(1,sizeof(rObject_t));
	if(!readFromFile(fileName, object)){
		free(object);
		return;
	}
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	char **screen = makeScreen();
	render(object, screen);
	for(int i = 0; i<w.ws_row; i++){
		printf("%s", screen[i]);
		free(screen[i]);
	}
	free(screen);
	free(object);
}

#endif
