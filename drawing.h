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

#include "colors.h"
#include "main.h"
#include "util.h"

extern int ttyFD;
extern int frameBufferFD;
extern struct fb_var_screeninfo screenInfo;
extern int frameBufferSize;
extern char *restrict frameBuffer;
extern char *restrict drawBuffer;

void drawInit();
void drawLoop();
void drawQuit();