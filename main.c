#include <stdio.h>

#include "drawing.h"
#include "util.h"

int argi;
char **args;

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("Provide an obj model!\n");
		exit(EXIT_FAILURE);
	}

	argi = argc;
	args = argv;

	backend = BACKEND_X11;

	drawInit();

	while (1) {
		drawLoop();
	}

	drawQuit();

	return 0;
}