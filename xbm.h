#pragma once

#define FONT_WIDTH		  8
#define FONT_HEIGHT		  13
#define FONT_ASCII_OFFSET 33

#include "util.h"

extern const char ascii_8x13_font[94][13];

void drawText(const char *text, int len, int x, int y, Color color);