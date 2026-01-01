#pragma once

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>

#include "colors.h"
#include "main.h"
#include "util.h"

typedef enum { BACKEND_FB, BACKEND_X11 } GUI_BACKEND;

extern GUI_BACKEND backend;

extern int backendFB_ttyFD;
extern int backendFB_frameBufferFD;
extern struct fb_var_screeninfo backendFB_screenInfo;

extern const int backendX11_WindowWidth;
extern const int backendX11_WindowHeight;
extern Display *backendX11_display;
extern int backendX11_screen;
extern Window backendX11_window;
extern GC backendX11_gc;
extern XEvent backendX11_event;
extern XImage *backendX11_image;

extern int frameBufferSize;
extern char *restrict frameBuffer;
extern char *restrict drawBuffer;

void drawInit();
void drawLoop();
void drawQuit();