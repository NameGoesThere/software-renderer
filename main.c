#include <stdio.h>

#include "drawing.h"
#include "util.h"

int argi;
char **args;

int main(int argc, char *argv[]) {

	argi = argc;
	args = argv;

	drawInit();

	while (1) {
		drawLoop();
	}

	drawQuit();

	return 0;
}