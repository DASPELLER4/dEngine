#include "include/string.h"
#include "include/split.h"
#include "include/execute.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	char inputBuffer[255] = {0};
	printf("Welcome to dEngine\ntype help to see the help message\n");
	while(strcmp(inputBuffer,"quit")){
		printf("> ");
		fgets(inputBuffer, sizeof(inputBuffer), stdin);
		inputBuffer[strcspn(inputBuffer, "\n")] = 0;
		split_t* split = splitBy(inputBuffer, ' ');
		execute(split);
		free(split);
	}
}
