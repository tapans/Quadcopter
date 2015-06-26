
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


bool stringContains(char* fullString, char* substring) {

	if(strncasecmp(fullString, substring, strlen(substring)) == 0) {
		return true;
	}
	return false;
}

int main(void) {


	if(stringContains("hello world", "hello") == true) {
		printf("%s\n", "matched");
	}else {
		printf("%s\n", "failed to match");
	}

	//char *str  = stripLettersFromFloat("asdaksjhdk0.567810273123asdjkhas");
	//printf("%s\n", str);
	//free(str);
	return 0;
}