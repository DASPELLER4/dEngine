#ifndef SPLIT_H
#define SPLIT_H

#define MAXWORDS 32
#define MAXWORDLENGTH 32

#include <stdlib.h>
#include <string.h>

typedef struct split{
	char splits[MAXWORDS][MAXWORDLENGTH];
	int length;
} split_t;

split_t *splitBy(char *text, char token){
	split_t *split = calloc(1,sizeof(split_t));
	int len = strlen(text);
	int curr,i,j; // curr = current word, i = current character in text, j = current character in word
	for(curr = i = j = 0; curr<MAXWORDS && i<len; i++){
		if(j==MAXWORDLENGTH){ // overflow of word length
			split->splits[curr][j] = 0;
			j = 0;
			curr++;
		}
		else if(text[i]!=token){
			split->splits[curr][j] = text[i];
			j++;
		}else{
			split->splits[curr][j] = 0;
			curr++;
			j = 0;
		}
	}
	split->length = curr+1;
	return split;
}

#endif
