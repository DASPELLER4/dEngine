main:
	gcc main.c -o dEngine -lGL -lglut

install: dEngine
	mv dEngine /usr/bin/dEngine
