#include "obj.h"

Object *objLoadObject(char *path) {
	FILE *obj = fopen(path, "r");
	if (!obj)
		return NULL;

	Object *object = calloc(1, sizeof(Object));

	char *line = malloc(OBJ_LINE_MAX);
	while (fgets(line, OBJ_LINE_MAX, obj)) {
		if (line[0] == '#')
			continue;

		char *tok = strtok(line, " ");

		PARSE_MODE mode = NONE;
		Vec3 storedVertex = {0};
		int storedTriangle[3] = {0};

		for (int i = 0; tok; ++i) {
			if (!i) {
				if (!strcmp(tok, "v"))
					mode = VERTEX;
				else if (!strcmp(tok, "f"))
					mode = FACE;
				else
					mode = NONE;
			} else {
				switch (mode) {
				case VERTEX:
					storedVertex.values[i - 1] = atof(tok);
					break;
				case FACE:
					storedTriangle[i - 1] = atoi(tok) - 1;
					break;
				default:
					break;
				}
			}

			tok = strtok(NULL, " ");
		}

		switch (mode) {
		case VERTEX:
			object->points =
				realloc(object->points, sizeof(Vec3) * ++object->numPoints);
			object->points[object->numPoints - 1] = storedVertex;
			break;
		case FACE:
			object->triangles = realloc(
				object->triangles, sizeof(int[3]) * ++object->numTriangles);
			memcpy(object->triangles[object->numTriangles - 1], storedTriangle,
				   sizeof(int[3]));
			break;
		default:
			break;
		}
	}

	if (line)
		free(line);

	fclose(obj);

	return object;
}