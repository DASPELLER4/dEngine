#ifndef M_STRING_H
#define M_STRING_H

#include <stdlib.h>
#include <string.h>

typedef struct string{
	char value;
	struct string *next;
} string_t;

string_t *findLastCharacter(string_t *string){
	string_t *curr = string;
	while(curr->next){
		curr = curr->next;		
	}
	return curr;
}

void appendCharacter(string_t **string, char ch){
	if(*string){
		string_t *last = findLastCharacter(*string);
		string_t *new = malloc(sizeof(string_t));
		last->next = new;
		new->value = ch;
		new->next = NULL;
	} else {
		*string = malloc(sizeof(string_t));
		(*string)->value = ch;
		(*string)->next = NULL;
	}
}

void freeString(string_t *string){
	if(string){
		string_t *curr = string;
		string_t *next;
		while(curr){
			next = curr->next;
			free(curr);
			curr = next;
		}
	}
}

void appendCharPtr(char *source, string_t *dest, short ifToFreeOrNotToFreeThatIsTheQuestion){
	int len = strlen(source);
	for(int i = 0; i < len; i++){
		appendCharacter(&dest, source[i]);
	}
	if(ifToFreeOrNotToFreeThatIsTheQuestion){
		free(source);	
	}
} 

void appendString(string_t *source, string_t *dest){
	findLastCharacter(dest)->next = source;
}

string_t *defineEmptyString(){
	return NULL;
}

string_t *defineStringFromCharPtr(char *start, short ifToFreeOrNotToFreeThatIsTheQuestion){
	string_t *string = NULL;
	appendCharPtr(start, string, ifToFreeOrNotToFreeThatIsTheQuestion);
	if(ifToFreeOrNotToFreeThatIsTheQuestion){
		free(start);
	}
	return string;
}

string_t *copyString(string_t *string){
	string_t *curr = string;
	string_t *ret = NULL;
	while(curr){
		appendCharacter(&ret, curr->value);
		curr = curr->next;
	}
	return ret;
}

int stringLen(string_t *string){
	string_t *curr = string;
	int i = 0;
	while(curr){
		i++;
		curr = curr->next;
	}
	return i;
}

char *typeCastToCharPtr(string_t *string){
	char *ret = calloc(stringLen(string)+1, 1);
	string_t *curr = string;
	int i;
	for(i = 0; curr; i++){
		ret[i] = curr->value;
		curr = curr->next;
	}
	return ret;
}

#endif
