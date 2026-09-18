#include "revert_string.h"
#include <stdlib.h>
#include <string.h>

void RevertString(char *str)
{
	size_t len = strlen(str);
	char* result = calloc(len, sizeof(char));
	for (int i=0; i<len; i++){
		result[i] = str[len-i-1];
	}
	strcpy(str, result);
}

