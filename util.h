#pragma once

#include "drawing.h"

#include <math.h>

#define NEAR_PLANE 0.1

#define swap(x, y)                                                             \
	do {                                                                       \
		unsigned char                                                          \
			swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1];        \
		memcpy(swap_temp, &y, sizeof(x));                                      \
		memcpy(&y, &x, sizeof(x));                                             \
		memcpy(&x, swap_temp, sizeof(x));                                      \
	} while (0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef union {
	struct {
		float x, y;
	};

	float values[2];
} Vec2;

typedef union {
	struct {
		float x, y, z;
	};

	float values[3];
} Vec3;

typedef struct {
	unsigned char red, green, blue;
} Color;

typedef struct {
	int points[3];
	int normal;
} face;

typedef struct {
	Vec3 *points;
	int numPoints;

	face *faces;
	int numFaces;

	Vec3 *normals;
	int numNormals;
} Object;

typedef struct {
	int index;
	float maxZ;
} ZOrderObject;

Vec2 screenToNormalized(Vec2 point);
Vec2 normalizedToScreen(Vec2 point);
Vec2 normalizedToScreen3D(Vec3 point);
Vec3 rotateX(Vec3 point, float angle);
Vec3 rotateY(Vec3 point, float angle);
Vec3 rotateZ(Vec3 point, float angle);

void clear();
void drawPixel(int x, int y, Color color);
void drawLine(int x1, int y1, int x2, int y2, Color color);
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color);
void drawObject(Object *object, Color color);
void destroyObject(Object *object);

int pointInBounds(int x, int y);
int compareZOrder(const void *a, const void *b);