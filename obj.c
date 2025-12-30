#include "obj.h"

Object *objLoadObject(char *path) {
	FILE *obj = fopen(path, "r");
	if (!obj)
		return NULL;

	Object *object = calloc(1, sizeof(Object));

	char *line = malloc(OBJ_LINE_MAX);

	while (fgets(line, OBJ_LINE_MAX, obj)) {
		if (line[0] == '#' || line[0] == '\n')
			continue;

		char *tok = strtok(line, " ");

		if (!strcmp(tok, "v"))
			++object->numPoints;
		else if (!strcmp(tok, "f"))
			++object->numFaces;
		else if (!strcmp(tok, "vn"))
			++object->numNormals;
	}

	object->points = malloc(sizeof(Vec3) * object->numPoints);
	object->faces = malloc(sizeof(face) * object->numFaces);
	object->normals = malloc(sizeof(Vec3) * object->numNormals);

	int pointIndex = 0;
	int faceIndex = 0;
	int normalIndex = 0;

	rewind(obj);
	while (fgets(line, OBJ_LINE_MAX, obj)) {
		if (line[0] == '#' || line[0] == '\n')
			continue;

		char *tok = strtok(line, " ");

		PARSE_MODE mode = NONE;
		Vec3 storedVertex = {0};
		int storedFace[3] = {0};
		Vec3 storedNormal = {0};
		int storedNormalIndex = 0;

		for (int i = 0; tok; ++i) {
			if (!i) {
				if (!strcmp(tok, "v"))
					mode = VERTEX;
				else if (!strcmp(tok, "f"))
					mode = FACE;
				else if (!strcmp(tok, "vn"))
					mode = NORMAL;
				else
					mode = NONE;
			} else {
				char subString[strlen(tok) + 1];
				char *subTok;
				char *save;

				switch (mode) {
				case VERTEX:
					storedVertex.values[i - 1] = atof(tok);
					break;
				case FACE:
					strcpy(subString, tok);
					subTok = strtok_r(subString, "/", &save);

					for (int j = 0; subTok; ++j) {
						switch (j) {
						case 0:
							storedFace[i - 1] = atoi(subTok) - 1;
							break;
						case 2:
							storedNormalIndex = atoi(subTok) - 1;
						default:
							break;
						}

						subTok = strtok_r(NULL, "/", &save);
					}
					break;
				case NORMAL:
					storedNormal.values[i - 1] = atof(tok);
					break;
				default:
					break;
				}
			}

			tok = strtok(NULL, " ");
		}

		switch (mode) {
		case VERTEX:
			object->points[pointIndex++] = storedVertex;
			break;
		case FACE:
			memcpy(object->faces[faceIndex++].points, storedFace,
				   sizeof(storedFace));
			object->faces[faceIndex - 1].normal = storedNormalIndex;
			break;
		case NORMAL:
			object->normals[normalIndex++] = storedNormal;
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