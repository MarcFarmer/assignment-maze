#include "Cube.h"
#include "Maze.h"
#include "Sphere.hpp"

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GLFW/glfw3.h>

#define RENDER_STAGE_FLOOR 0
#define RENDER_STAGE_BLOCKS 1
#define RENDER_STAGE_GOAL 2
#define RENDER_STAGE_BALL 3

int cubeIndicesCount;
int sphereIndicesCount;

unsigned int cubeVaoHandle;
unsigned int sphereVaoHandle;

/**
 * Creates a new vertex array object
 * and loads in data into a vertex attribute buffer
 *
 * @param handle Pointer to VAO handle to bind when drawing this object
 * @param vertices Vertices of object
 * @param vertCount Number of vertices
 * @param valsPerVert Number of coordinate values per vertex
 * @param indices Indices of object
 * @param indCount Number of indices
 */
void Maze::createVAO(unsigned int* handle, 
	float* vertices, int vertCount, int valsPerVert, 
	unsigned int* indices, int indCount)
	{
	// Generate storage for VAO and make it current
	glGenVertexArrays(1, handle);
	glBindVertexArray(*handle);

	// Buffer for vertices and indices
	unsigned int buffer[2];
	glGenBuffers(2, buffer);

	// Set vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(float)*vertCount, 
		vertices, 
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, valsPerVert, GL_FLOAT, GL_FALSE, 0, 0);

	// Set element attributes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*indCount, 
		indices, 
		GL_STATIC_DRAW);   
}

/**
 * This helper traverses the grid and finds the x,y coordiates of blocks and the goal
 * Use these coordinates to draw blocks, 
 * instead of always traversing the grid to find blocks and goal
 */
void Maze::setupItemCoordinates() {
	std::string mazeLine;

	for (int row = 0; row < grid.size(); row++) {
		mazeLine = grid.at(row);

		for (int col = 0; col < grid.size(); col++) {
			if (mazeLine[col] == '*') {
				blocks.push_back(row);
				blocks.push_back(col);
			} else if (mazeLine[col] == 'X') {
				goalX = row;
				goalY = col;
			}
		}
	}
}

/**
 * This helper sets the uniform handles and values required for the maze
 */
void Maze::setupUniformVars() {
	int squareRadiusHandle = glGetUniformLocation(programID, "squareRadius");
	int sphereRadiusHandle = glGetUniformLocation(programID, "sphereRadius");
	renderStageUniformHandle = glGetUniformLocation(programID, "renderStage");
	modelUniformHandle = glGetUniformLocation(programID, "model");

	if (squareRadiusHandle == -1 || sphereRadiusHandle == -1 || 
		renderStageUniformHandle == -1 || modelUniformHandle == -1) {
		exit(1);
	}

	glUniform1f(squareRadiusHandle, 0.4f*cubeWidth);
	glUniform1f(sphereRadiusHandle, sphereRadius);
}

/**
 * This helper creates a transform positioned at the top left of the maze
 * Used to render the maze starting from the top left corner
 */
void Maze::setupStartingTransform(float mazeWidth) {
	startingTransform = glm::mat4(1.0f);
	glm::vec3 topLeft;

	if (grid.size() % 2 == 0) {
		topLeft = glm::vec3(-(mazeWidth/2.0f) + cubeWidth/2.0f, 
			0.0f, 
			-(mazeWidth/2.0f) + cubeWidth/2.0f
		);
	} else {
		topLeft = glm::vec3(-(mazeWidth/2.0f) + cubeWidth/2.0f, 
			0.0f, 
			-(mazeWidth/2.0f) + cubeWidth/2.0f
		);
	}

	startingTransform = glm::translate(startingTransform, topLeft);
}

/**
 * This helper creates the VAOs for objects in the maze, ready to bind before drawing
 */
void Maze::setupVAOs() {
	Cube* cube = new Cube(cubeWidth);
	Sphere* sphere = new Sphere(sphereRadius);

	cubeIndicesCount = cube->indCount;
	sphereIndicesCount = sphere->indCount;

	createVAO(&cubeVaoHandle, 
		cube->vertices, cube->vertCount, cube->valsPerVert, 
		cube->indices, cube->indCount);

	createVAO(&sphereVaoHandle, 
		sphere->vertices, sphere->vertCount, sphere->valsPerVert, 
		sphere->indices, sphere->indCount);

	delete cube;
	delete sphere;
}

/**
 * Initialise variables required to render the maze
 * @param grid Grid of characters that specify the maze
 * @param mazeWidth Width of each side of the maze
 * @param programID Loaded shader program
 */
Maze::Maze(std::vector<std::string> grid, float mazeWidth, unsigned int programID):
	grid(grid),
	programID(programID),
	ballX(0),
	ballY(0) {

	// Use shader program
	glUseProgram(programID);

	// Calculate dimensions of cube, sphere and squares based on maze width and grid size
	cubeWidth = mazeWidth/(float)grid.size();
	sphereRadius = 0.43f*cubeWidth;

	// Find x,y coordinates of blocks and goal in the grid
	setupItemCoordinates();

	// Create a transform at the top left of the maze
	setupStartingTransform(mazeWidth);

	// Get uniform handles and set values
	setupUniformVars();

	// Create VAOs of shapes needed for the maze
	setupVAOs();
}


/**
 * Bind cube VAO, draw a cube that can be modified by a transformation matrix
 * @param transform Translate, rotate and scale cube
 */
void Maze::drawCube(glm::mat4 transform) {
	glBindVertexArray(cubeVaoHandle);

	glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(transform));
	glDrawElements(GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, 0);

	unbindAfterDraw();
}

/**
 * Bind sphere VAO, draw a sphere that can be modified by a transformation matrix
 * @param transform Translate, rotate and scale cube
 */
void Maze::drawSphere(glm::mat4 transform) {
	glBindVertexArray(sphereVaoHandle);

	glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(transform));
	glDrawElements(GL_TRIANGLES, sphereIndicesCount, GL_UNSIGNED_INT, 0);

	unbindAfterDraw();
}

/**
 * Render the maze floor
 * Draw one cube (with reduced height) for each grid square in the maze
 */
void Maze::renderFloor() {
	glUniform1i(renderStageUniformHandle, RENDER_STAGE_FLOOR);

	glm::mat4 floorTransform;
	for (int row = 0; row < grid.size(); row++) {
		for (int col = 0; col < grid.size(); col++) {
			// Move floor cube to current grid square
			floorTransform = glm::translate(startingTransform, 
				glm::vec3((float)row * cubeWidth, 0.0f, (float)col * cubeWidth));
			// Reduce the height of the cube and move it below the XZ plane
			floorTransform = glm::scale(floorTransform, glm::vec3(1.0f, 0.5f, 1.0f));
			floorTransform = glm::translate(floorTransform, 
				glm::vec3(0.0f, -0.5f*cubeWidth, 0.0f));

			drawCube(floorTransform);
		}
	}
}

/**
 * Render the maze blocks
 * Draw one cube for each block, placed on top of the floor
 */
void Maze::renderBlocks() {
	glUniform1i(renderStageUniformHandle, RENDER_STAGE_BLOCKS);
 
	glm::mat4 blockTransform;
	for (int i = 0; i < blocks.size(); i = i+2) {
		// Move floor cube to current grid square and move it up so it sits on the XZ plane
		blockTransform = glm::translate(startingTransform, 
			glm::vec3((float)blocks.at(i+1) * cubeWidth, cubeWidth/2.0f, (float)blocks.at(i) * cubeWidth));

		drawCube(blockTransform);
	}

}

/**
 * Render the goal
 * Draw a sphere above the floor at the goal position
 */
void Maze::renderGoal() {
	glUniform1i(renderStageUniformHandle, RENDER_STAGE_GOAL);

	// Move goal sphere to the goal position, move it up above the floor
	glm::mat4 goalTransform = glm::translate(startingTransform, 
		glm::vec3((float)goalY*cubeWidth, 0.6f*cubeWidth, (float)goalX*cubeWidth));

	drawSphere(goalTransform);
}

/**
 * Render the ball
 * Draw a sphere above the floor at the ball position
 */
void Maze::renderBall() {
	glUniform1i(renderStageUniformHandle, RENDER_STAGE_BALL);

	// Move goal sphere to the position the player moved to, move it up above the floor
	glm::mat4 ballTransform = glm::translate(startingTransform, 
		glm::vec3((float)ballY*cubeWidth, 0.6f*cubeWidth, (float)ballX*cubeWidth));

	drawSphere(ballTransform);
}

/**
 * Unbind VAOs and buffers
 */
void Maze::unbindAfterDraw() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glFlush();
}

/**
 * Draw the maze based on the maze input file.
 */
void Maze::render() {
	renderFloor();
	renderBlocks();
	renderGoal();
	renderBall();
}
