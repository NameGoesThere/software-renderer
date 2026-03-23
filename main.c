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
	if (argc < 3) {
		printf("Provide a backend (X11 OR FB)!\n");
		exit(EXIT_FAILURE);
	}

	backend = strcmp(argv[2], "X11") ? BACKEND_FB : BACKEND_X11;

	argi = argc;
	args = argv;

	drawInit();

	while (1) {
		drawLoop();
	}

	drawQuit();

	return 0;
}