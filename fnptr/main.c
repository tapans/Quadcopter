#include <stdio.h>
#include "other.h"

//Function to be called
void helloWorld(numbers* n) {

	printf("%u\n", n->val);
}

int main(void) {

	//create the function pointer
	void (*helloFnc)(numbers*);

	//point it to the desired function
	helloFnc = helloWorld;

	init();
	//pass the pointer and the argument
	handle(helloFnc);
}