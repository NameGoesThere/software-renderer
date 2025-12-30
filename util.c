#include "util.h"

inline Vec2 screenToNormalized(Vec2 point) {
	return (Vec2){.values = {point.x / screenInfo.xres * 2.0 - 1,
							 point.y / screenInfo.yres * 2.0 - 1}};
}

inline Vec2 normalizedToScreen(Vec2 point) {
	return (Vec2){.values = {(point.x + 1) / 2.0 * screenInfo.xres,
							 (point.y + 1) / 2.0 * screenInfo.yres}};
}

inline Vec2 normalizedToScreen3D(Vec3 point) {
	float reciprocal = 1.0 / point.z;

	return normalizedToScreen(
		(Vec2){.values = {point.x * reciprocal, point.y * reciprocal}});
}

inline Vec3 rotateX(Vec3 point, float angle) {
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	return (Vec3){.values = {point.x, point.y * cosAngle - point.z * sinAngle,
							 point.y * sinAngle + point.z * cosAngle}};
}

inline Vec3 rotateY(Vec3 point, float angle) {
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	return (Vec3){.values = {point.x * cosAngle + point.z * sinAngle, point.y,
							 -point.x * sinAngle + point.z * cosAngle}};
}

inline Vec3 rotateZ(Vec3 point, float angle) {
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	return (Vec3){.values = {point.x * cosAngle - point.y * sinAngle,
							 point.x * sinAngle + point.y * cosAngle, point.z}};
}

void clear() { memset(drawBuffer, 0, frameBufferSize); }

void drawPixel(int x, int y, Color color) {
	if (!pointInBounds(x, y))
		return;

	int offset = (y * screenInfo.xres + x) * 4;

	drawBuffer[offset] = color.blue;
	drawBuffer[offset + 1] = color.green;
	drawBuffer[offset + 2] = color.red;
	drawBuffer[offset + 3] = 0;
}

void drawLine(int x1, int y1, int x2, int y2, Color color) {
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	int steep = 0;

	if (dy > dx) {
		steep = 1;
		swap(x1, y1);
		swap(x2, y2);
		swap(dx, dy);
	}

	if (x1 > x2) {
		swap(x1, x2);
		swap(y1, y2);
	}

	int error = dx / 2;
	int y = y1;

	int yStep = (y1 < y2) ? 1 : -1;

	for (int x = x1; x <= x2; ++x) {
		if (steep)
			drawPixel(y, x, color);
		else
			drawPixel(x, y, color);

		error -= dy;
		if (error < 0) {
			y += yStep;
			error += dx;
		}
	}
}

float rotateAmount = 0;
float moveAmount = 1.5;

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
	drawLine(x1, y1, x2, y2, WHITE);
	drawLine(x1, y1, x3, y3, WHITE);
	drawLine(x2, y2, x3, y3, WHITE);

	int minX = MIN(MIN(x1, x2), x3);
	int maxX = MAX(MAX(x1, x2), x3);
	int minY = MIN(MIN(y1, y2), y3);
	int maxY = MAX(MAX(y1, y2), y3);

	if (y1 == y3) {
		swap(y1, y2);
		swap(x1, x2);
	}

	for (int pointX = minX; pointX <= maxX; ++pointX) {
		for (int pointY = minY; pointY <= maxY; ++pointY) {
			int s1 = y3 - y1;
			int s2 = x3 - x1;
			int s3 = y2 - y1;
			int s4 = pointY - y1;

			float w1 = (x1 * s1 + s4 * s2 - pointX * s1) /
					   (float)(s3 * s2 - (x2 - x1) * s1);

			if (w1 < 0)
				continue;

			float w2 = (s4 - w1 * s3) / (float)s1;

			if (w2 < 0 || (w1 + w2) > 1)
				continue;

			drawPixel(pointX, pointY, color);
		}
	}
}

void drawObject(Object *object, Color color) {
	if (!object)
		return;

	ZOrderObject *zOrdering =
		calloc(object->numTriangles, sizeof(ZOrderObject));

	for (int i = 0; i < object->numTriangles; ++i) {
		int *triangle = object->triangles[i];

		Vec3 pointA = object->points[triangle[0]];
		Vec3 pointB = object->points[triangle[1]];
		Vec3 pointC = object->points[triangle[2]];

		zOrdering[i] = (ZOrderObject){
			.index = i, .maxZ = (pointA.z + pointB.z + pointC.z) / 3.0};
	}

	qsort(zOrdering, object->numTriangles, sizeof(ZOrderObject), compareZOrder);

	rotateAmount += 0.02;
	moveAmount += 0.01;
	for (int i = 0; i < object->numTriangles; ++i) {
		int *triangle = object->triangles[zOrdering[i].index];

		Vec3 pointA = object->points[triangle[0]];
		Vec3 pointB = object->points[triangle[1]];
		Vec3 pointC = object->points[triangle[2]];

		pointA.z += moveAmount;
		pointB.z += moveAmount;
		pointC.z += moveAmount;

		if (pointA.z < NEAR_PLANE || pointB.z < NEAR_PLANE ||
			pointC.z < NEAR_PLANE)
			continue;

		Vec3 ab = {.values = {pointB.x - pointA.x, pointB.y - pointA.y,
							  pointB.z - pointA.z}};
		Vec3 ac = {.values = {pointC.x - pointA.x, pointC.y - pointA.y,
							  pointC.z - pointA.z}};
		float nz = ab.x * ac.y - ab.y * ac.x;
		if (nz >= 0)
			continue;

		Vec2 projectedA = normalizedToScreen3D(pointA);
		Vec2 projectedB = normalizedToScreen3D(pointB);
		Vec2 projectedC = normalizedToScreen3D(pointC);

		drawTriangle(projectedA.x, projectedA.y, projectedB.x, projectedB.y,
					 projectedC.x, projectedC.y, color);
	}

	free(zOrdering);
}

void destroyObject(Object *object) {
	free(object->points);
	free(object->triangles);
	free(object);
}

inline int pointInBounds(int x, int y) {
	return !(x < 0 || x >= screenInfo.xres || y < 0 || y >= screenInfo.yres);
}

int compareZOrder(const void *a, const void *b) {
	const ZOrderObject *objA = (const ZOrderObject *)a;
	const ZOrderObject *objB = (const ZOrderObject *)b;

	if (objB->maxZ > objA->maxZ)
		return 1;

	if (objA->maxZ > objB->maxZ)
		return -1;

	return 0;
}