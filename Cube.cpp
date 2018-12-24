#include "Cube.h"

#include <cstring>
#include <stdio.h>

#define NUM_TRIS 12
#define VALS_PER_VERT 3

/*
 * Define cube vertices and indices
 * We define vertices and indices with an intialiser list for readability
 * Use memcpy to store vertices in a dynamic variable, to avoid dangling pointer problem
 * @param width Width of each cube face
 */
Cube::Cube(float width): 
	vertCount(NUM_TRIS * VALS_PER_VERT), 
	numTris(NUM_TRIS), 
	valsPerVert(VALS_PER_VERT), 
	indCount(NUM_TRIS*3) {

	// Create vertices with given width
	float cubeVertices[NUM_TRIS * VALS_PER_VERT] = {
		-width/2.0f, -width/2.0f,  width/2.0f,
		width/2.0f, -width/2.0f,  width/2.0f,
		width/2.0f,  width/2.0f,  width/2.0f,
		-width/2.0f,  width/2.0f,  width/2.0f,
		-width/2.0f, -width/2.0f, -width/2.0f,
		width/2.0f, -width/2.0f, -width/2.0f,
		width/2.0f,  width/2.0f, -width/2.0f,
		-width/2.0f,  width/2.0f, -width/2.0f
	};

	vertices = new float[NUM_TRIS * VALS_PER_VERT];
	std::memcpy(vertices, cubeVertices, NUM_TRIS * VALS_PER_VERT * sizeof(float));

	// 12 triangles - 2 per face of the cube
	unsigned int cubeIndices[NUM_TRIS * 3] = {
		0,1,2, 2,3,0,
		1,5,6, 6,2,1,
		5,4,7, 7,6,5,
		4,0,3, 3,7,4,
		3,2,6, 6,7,3,
		4,5,1, 1,0,4
	};

	indices = new unsigned int[NUM_TRIS * 3];
	std::memcpy(indices, cubeIndices, NUM_TRIS * 3 * sizeof(unsigned int));
}

Cube::~Cube() {
	delete []vertices;
	delete []indices;
}