#pragma once

#include "drawing.h"
#include "util.h"

#define OBJ_LINE_MAX 256

typedef enum { NONE, VERTEX, FACE, NORMAL } PARSE_MODE;

Object *objLoadObject(char *path);