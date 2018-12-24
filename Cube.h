/**
 * Generates cube vertices and indices.
 */

#ifndef CUBE_H
#define CUBE_H

class Cube {
public:
	Cube(float width = 1.0f);

	~Cube();

	int vertCount;
	int numTris;
	int valsPerVert;
	int indCount;

	float *vertices;
	unsigned int *indices;
};

#endif