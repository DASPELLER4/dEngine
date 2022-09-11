#ifndef POLYEDITOR_H
#define POLYEDITOR_H

#define ELENOFLINE 64

#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#include "../config.h"

typedef struct eVector2d{
	double a;
	double b;
} eVector2d_t;

typedef struct ePolygon{
	eVector2d_t v[3];
} ePolygon_t;

typedef struct eObject{
	int count;
	int brightness;
	ePolygon_t polygons[MAXPOLYGONS];
} eObject_t;

eObject_t *gobject;
int *gsize;

void addVert(int x, int y, short clear){
	static int currVert = 0;
	if(clear){
		currVert = 0;
		return;
	}
	if(gobject->count >= MAXPOLYGONS-1){
		printf("You have exceeded the maximum polygons available, to support larger object complexity\nIncrease the MAXPOLYGONS macro in config.h and re-run make\n");
		return;
	}
	gobject->polygons[gobject->count].v[currVert].a = ((double)x)/((double)glutGet(GLUT_WINDOW_WIDTH))*((double)glutGet(GLUT_WINDOW_WIDTH)/(double)glutGet(GLUT_WINDOW_HEIGHT));
	gobject->polygons[gobject->count].v[currVert].b = ((double)y)/((double)glutGet(GLUT_WINDOW_HEIGHT));
	currVert+=1;
	if(currVert>=3){
		currVert = 0;
		gobject->count+=1;
	}
}

int removePoly(){
	gobject->count -= 1;
	if(gobject->count < 0)
		gobject->count = 0;
	return gobject->count;
}

void mouseClicks(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		addVert(x, y, 0);
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		removePoly();
	}
}

void keypress(unsigned char key, int x, int y){
	switch(key){
		case('q'):
			glutDestroyWindow(glutGetWindow());
			break;
		case('r'):
			while(removePoly());
			addVert(x, y, 1);
			break;
		case('h'):
			printf("Object Generator Help Page\n\tThis tool is to assist in the creation of a 2D object\n\nControls:\n\tLMB-Places a new vertex, takes 3 vertecies to create a polygon\n\tRMB-Delete last polygon\n\tQ-Save and quit the software\n\tR-Reset all polygons\n\tH-Display this page\n");
			break;
		default:
			printf("Unrecognized keypress (\'%c\').\nPress H to show the help page\n", key);
	}
}

void idle(){
	glutPostRedisplay();
}

void display(){
	gsize[0] = glutGet(GLUT_WINDOW_WIDTH);
	gsize[1] = glutGet(GLUT_WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	for(int i = 0; i<gobject->count; i++){
		float stdBrightness = ((float)gobject->brightness)/5.0f;
		glColor3f(stdBrightness, stdBrightness, stdBrightness);
		glBegin(GL_POLYGON);
			glVertex3f(gobject->polygons[i].v[0].a*2-1, -(gobject->polygons[i].v[0].b*2-1), 0);
			glVertex3f(gobject->polygons[i].v[1].a*2-1, -(gobject->polygons[i].v[1].b*2-1), 0);
			glVertex3f(gobject->polygons[i].v[2].a*2-1, -(gobject->polygons[i].v[2].b*2-1), 0);
		glEnd();
	}
	glFlush();
}

eObject_t *polyEditor(int brightness){
	char intToCharBrightness[5] = {'.','<','|','X','$'};
	gobject = calloc(1,sizeof(eObject_t));
	gobject->brightness = brightness;
	gsize = malloc(2*sizeof(int));
	//make up the args for glutInit
	int x = 1;
	char **j = malloc(sizeof(char*));
	j[0] = malloc(2);
	j[0][0] = 'a';
	j[0][1] = 0;
	printf("Object Generator Help Page\n\tThis tool is to assist in the creation of a 2D object\n\nControls:\n\tLMB-Places a new vertex, takes 3 vertecies to create a polygon\n\tRMB-Delete last polygon\n\tQ-Save and quit the software\n\tR-Reset all polygons\n\tH-Display this page\n");
	//start opengl
	glutInit(&x, j);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Object Creator | h for Help");
	glClear(GL_COLOR_BUFFER_BIT);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouseClicks);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
	free(j[0]);
	free(j);
	//formulate response
	for(int i = 0; i < gobject->count; i++){
		gobject->polygons[i].v[0].a *= OBJECTSCALE;
		gobject->polygons[i].v[0].b *= OBJECTSCALE;
		gobject->polygons[i].v[1].a *= OBJECTSCALE;
		gobject->polygons[i].v[1].b *= OBJECTSCALE;
		gobject->polygons[i].v[2].a *= OBJECTSCALE;
		gobject->polygons[i].v[2].b *= OBJECTSCALE;
	}
	free(gsize);
	gobject->brightness = intToCharBrightness[gobject->brightness-1];
	return gobject; 
}

#endif
