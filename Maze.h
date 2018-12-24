/**
 * Draw the maze by scaling and moving cubes and spheres.
 * The centre of the maze is at (0,0,0).
*/

#ifndef MAZE_H
#define MAZE_H

#include <string>
#include <vector>

#include "glm/glm.hpp"

class Maze {
private:
	std::vector<int> blocks;

	float cubeWidth, sphereRadius;
	glm::mat4 startingTransform;

	unsigned int programID;
	int modelUniformHandle, renderStageUniformHandle;

	void createVAO(unsigned int* handle, 
		float* vertices, int vertCount, int valsPerVert, 
		unsigned int* indices, int indCount);

	// render the maze
	void drawCube(glm::mat4 transform);
	void drawSphere(glm::mat4 transform);
	void renderFloor();
	void renderBlocks();
	void renderGoal();
	void renderBall();
	void unbindAfterDraw();

	// helpers
	void setupItemCoordinates();
	void setupStartingTransform(float mazeWidth);
	void setupUniformVars();
	void setupVAOs();

public:
	int ballX, ballY, goalX, goalY;
	std::vector<std::string> grid;

	Maze(std::vector<std::string> grid, float mazeWidth, unsigned int programID);
	void render();
};

#endif