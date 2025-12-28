#include "drawing.h"

#include "obj.h"
#include "xbm.h"

int ttyFD;
int frameBufferFD;
struct fb_var_screeninfo screenInfo;
int frameBufferSize;
char *restrict frameBuffer;
char *restrict drawBuffer;

Object *object;

void drawInit() {
	frameBufferFD = open("/dev/fb0", O_RDWR);
	if (frameBufferFD == -1)
		exit(1);

	ttyFD = open(ttyname(STDIN_FILENO), O_RDWR);
	if (ttyFD == -1)
		exit(1);

	ioctl(frameBufferFD, FBIOGET_VSCREENINFO, &screenInfo);
	ioctl(ttyFD, KDSETMODE, KD_GRAPHICS);

	screenInfo.activate |= FB_ACTIVATE_FORCE | FB_ACTIVATE_NOW;
	ioctl(frameBufferFD, FBIOPUT_VSCREENINFO, &screenInfo);

	frameBufferSize =
		screenInfo.xres * screenInfo.yres * screenInfo.bits_per_pixel / 8;

	frameBuffer = mmap(0, frameBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED,
					   frameBufferFD, (off_t)0);
	drawBuffer = calloc(sizeof(char), frameBufferSize);

	atexit(drawQuit);

	signal(SIGINT, exit);
	signal(SIGTERM, exit);
	signal(SIGSEGV, exit);

	if (argi > 1) {
		object = objLoadObject(args[1]);
	}
}

void drawLoop() {
	clear();

	drawObject(object, INDIGO);

	if (argi > 1) {
		drawText(args[1], strlen(args[1]), 5, 5, VIOLET);
	}

	memcpy(frameBuffer, drawBuffer, frameBufferSize);

	ioctl(frameBufferFD, FBIO_WAITFORVSYNC);
}

void drawQuit() {
	free(drawBuffer);
	munmap(frameBuffer, frameBufferSize);

	destroyObject(object);

	ioctl(ttyFD, KDSETMODE, KD_TEXT);

	close(ttyFD);
	close(frameBufferFD);
}
