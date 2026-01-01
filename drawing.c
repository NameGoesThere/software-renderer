#include "drawing.h"

#include "obj.h"
#include "xbm.h"

GUI_BACKEND backend = BACKEND_FB;

int backendFB_ttyFD;
int backendFB_frameBufferFD;
struct fb_var_screeninfo backendFB_screenInfo;

int frameBufferSize;
char *restrict frameBuffer;
char *restrict drawBuffer;

const int backendX11_WindowWidth = 1000;
const int backendX11_WindowHeight = 1000;
Display *backendX11_display;
int backendX11_screen;
Window backendX11_window;
GC backendX11_gc;
XEvent backendX11_event;
XImage *backendX11_image;

Object *object = NULL;

void drawInit() {
	switch (backend) {
	case BACKEND_FB:
		backendFB_frameBufferFD = open("/dev/fb0", O_RDWR);
		if (backendFB_frameBufferFD == -1)
			exit(1);

		backendFB_ttyFD = open(ttyname(STDIN_FILENO), O_RDWR);
		if (backendFB_ttyFD == -1)
			exit(1);

		ioctl(backendFB_frameBufferFD, FBIOGET_VSCREENINFO,
			  &backendFB_screenInfo);
		ioctl(backendFB_ttyFD, KDSETMODE, KD_GRAPHICS);

		backendFB_screenInfo.activate |= FB_ACTIVATE_FORCE | FB_ACTIVATE_NOW;
		ioctl(backendFB_frameBufferFD, FBIOPUT_VSCREENINFO,
			  &backendFB_screenInfo);

		frameBufferSize = backendFB_screenInfo.xres *
						  backendFB_screenInfo.yres *
						  backendFB_screenInfo.bits_per_pixel / 8;

		frameBuffer = mmap(0, frameBufferSize, PROT_READ | PROT_WRITE,
						   MAP_SHARED, backendFB_frameBufferFD, (off_t)0);
		drawBuffer = calloc(sizeof(char), frameBufferSize);
		break;
	case BACKEND_X11:
		frameBufferSize =
			backendX11_WindowWidth * backendX11_WindowHeight * 32 / 8;
		drawBuffer = calloc(sizeof(char), frameBufferSize);

		backendX11_display = XOpenDisplay((char *)0);
		backendX11_screen = DefaultScreen(backendX11_display);
		backendX11_window = XCreateSimpleWindow(
			backendX11_display, DefaultRootWindow(backendX11_display), 0, 0,
			backendX11_WindowWidth, backendX11_WindowHeight, 1, 1, 0);

		XSetStandardProperties(backendX11_display, backendX11_window,
							   "Software Renderer", "Software Renderer", None,
							   NULL, 0, NULL);

		backendX11_gc =
			XCreateGC(backendX11_display, backendX11_window, 0, NULL);

		backendX11_image = XCreateImage(
			backendX11_display,
			DefaultVisual(backendX11_display, backendX11_screen),
			DefaultDepth(backendX11_display, backendX11_screen), ZPixmap, 0,
			drawBuffer, backendX11_WindowWidth, backendX11_WindowHeight, 32, 0);

		XClearWindow(backendX11_display, backendX11_window);
		XMapRaised(backendX11_display, backendX11_window);

		XSelectInput(backendX11_display, backendX11_window,
					 ExposureMask | ButtonPressMask | KeyPressMask);
		break;
	default:
		break;
	}

	atexit(drawQuit);

	signal(SIGINT, exit);
	signal(SIGTERM, exit);
	signal(SIGSEGV, exit);

	if (argi > 1) {
		object = objLoadObject(args[1]);

		for (int i = 0; i < object->numPoints; ++i) {
			object->points[i] = rotateX(object->points[i], 3.1416);
			object->points[i].z += 2;
		}

		for (int i = 0; i < object->numNormals; ++i) {
			object->normals[i] = rotateX(object->normals[i], 3.1416);
		}
	}
}

void drawLoop() {
	if (backend == BACKEND_X11) {
		while (XPending(backendX11_display)) {
			XNextEvent(backendX11_display, &backendX11_event);
		}
	}

	clear();

	drawObject(object, INDIGO);

	for (int i = 0; i < object->numPoints; ++i) {
		object->points[i].z -= 2;
		object->points[i] = rotateY(object->points[i], 0.02);
		object->points[i].z += 2;
	}

	for (int i = 0; i < object->numNormals; ++i) {
		object->normals[i] = rotateY(object->normals[i], 0.02);
	}

	if (argi > 1) {
		drawText(args[1], strlen(args[1]), 5, 5, VIOLET);
	}

	switch (backend) {
	case BACKEND_FB:
		ioctl(backendFB_frameBufferFD, FBIO_WAITFORVSYNC);
		memcpy(frameBuffer, drawBuffer, frameBufferSize);
		break;
	case BACKEND_X11:
		XPutImage(backendX11_display, backendX11_window, backendX11_gc,
				  backendX11_image, 0, 0, 0, 0, backendX11_WindowWidth,
				  backendX11_WindowHeight);
		XFlush(backendX11_display);
		break;
	default:
		break;
	}
}

void drawQuit() {
	destroyObject(object);

	switch (backend) {
	case BACKEND_FB:
		free(drawBuffer);

		munmap(frameBuffer, frameBufferSize);

		ioctl(backendFB_ttyFD, KDSETMODE, KD_TEXT);

		close(backendFB_ttyFD);
		close(backendFB_frameBufferFD);
		break;
	case BACKEND_X11:
		backendX11_image->data = NULL;
		XDestroyImage(backendX11_image);
		free(drawBuffer);

		XFreeGC(backendX11_display, backendX11_gc);
		XDestroyWindow(backendX11_display, backendX11_window);
		XCloseDisplay(backendX11_display);
		break;
	default:
		break;
	}
}
