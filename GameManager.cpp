#include "GameManager.h"
#include "Maze.h"

#include <iostream>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>

// 4 directions the ball can move, relative to original camera position
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// used to find the approximate direction (NESW) the camera is facing
const float ROTATION_NORTH_WEST = M_PI/4.0f;
const float ROTATION_SOUTH_WEST = 3.0f*M_PI/4.0f;
const float ROTATION_SOUTH_EAST = 5.0f*M_PI/4.0f;
const float ROTATION_NORTH_EAST = 7.0f*M_PI/4.0f;

GameManager::GameManager(Maze* maze): maze(maze) {
	reset();
}

/**
 * Set the initial game state and ball position.
 */
void GameManager::reset() {
	maze->ballX = 0;
	maze->ballY = 0;
	moves = 0;
}

/**
 * Find direction camera is facing relative to the original camera position
 * Original camera position (no rotation) is facing NORTH
 * @param rotation How much the camera has been rotated anticlockwise.
 * 		  Measured in radians from 0 to 2*PI.
 */
int GameManager::findCameraDirection(float rotation) {
	if (rotation < ROTATION_NORTH_WEST || rotation > ROTATION_NORTH_EAST) {
		return NORTH;
	} else if (rotation < ROTATION_SOUTH_WEST) {
		return WEST;
	} else if (rotation < ROTATION_SOUTH_EAST) {
		return SOUTH;
	} else if (rotation < ROTATION_NORTH_EAST) {   // camera faces east
		return EAST;
	}
}

/**
 * Find direction to try to move in the grid
 * @param cameraRotation How much the camera has been rotated anticlockwise.
 * 		  Measured in radians from 0 to 2*PI.
 */
int GameManager::findGridDirection(int key, float cameraRotation) {
	// direction the player wants the ball to move, relative to the camera
	int keyDirection;
	switch(key) {
		case GLFW_KEY_RIGHT:
			keyDirection = EAST;
			break;
		case GLFW_KEY_UP:
			keyDirection = NORTH;
			break;
		case GLFW_KEY_LEFT:
			keyDirection = WEST;
			break;
		case GLFW_KEY_DOWN:
			keyDirection = SOUTH;
			break;
		default:
			break;
	}

	// find approxiamte direction the camera is facing, based on how much it rotated
	int cameraDirection = findCameraDirection(cameraRotation);

	// find direction for the ball to move in the grid
	// grid direction is defined: ball starts at (0,0) = NORTH WEST corner
	// default camera position (no rotation) faces NORTH
	// eg. if camera faces SOUTH, and user presses NORTH, ball moves SOUTH
	int gridDirection;
	if (cameraDirection == NORTH) {
		gridDirection = keyDirection;
	} else if (cameraDirection == WEST) {
		gridDirection = (keyDirection + 1) % 4;
	} else if (cameraDirection == SOUTH) {
		gridDirection = (keyDirection + 2) % 4;
	} else if (cameraDirection == EAST) {
		gridDirection = (keyDirection + 3) % 4;	    	
	}

	return gridDirection;
}

/**
 * Try to move the ball to a new grid position based on user input.
 * The ball movement is relative to the direction the camera is facing.
 * The user wins the game if the ball is moved to the goal.
 * @param key The GLFW keycode that was pressed by the user.
 * @param cameraRotation How much the camera has been rotated anticlockwise.
 * 		  Measured in radians from 0 to 2*PI.
 */
void GameManager::moveBall(int key, float cameraRotation) {
	char squareType;

	int newX = maze->ballX;
	int newY = maze->ballY;

	int gridDirection = findGridDirection(key, cameraRotation);
	int gridSize = maze->grid.size();

	// move one square at a time while we are not at the edge of the maze
	// stop moving if we reach a block
	switch(gridDirection) {
		case NORTH:
			while (newX > 0) {
				squareType = maze->grid.at(newX-1)[maze->ballY];
				if (squareType == '*') {    // next square is a block
					break;
				} else {
					newX = newX-1;  // move to empty square
				}
			}
			break;

		case SOUTH:
			while (newX < gridSize-1) {
				squareType = maze->grid.at(newX+1)[maze->ballY];
				if (squareType == '*') {    // next square is a block
					break;
				} else {
					newX = newX+1;  // move to empty square
				}
			}
			break;

		case EAST:
			while (newY < gridSize-1) {
				squareType = maze->grid.at(maze->ballX)[newY+1];
				if (squareType == '*') {    // next square is a block
					break;
				} else {
					newY = newY+1;  // move to empty square
				}
			}
			break;

		case WEST:
			while (newY > 0) {
				squareType = maze->grid.at(maze->ballX)[newY-1];
				if (squareType == '*') {    // next square is a block
					break;
				} else {
					newY = newY-1;  // move to empty square
				}
			}
			break;

		default:
			// You are standing in an open field west of a white house, 
			// with a boarded front door. There is a small mailbox here.
			break;
	}

	// update ball coordinates in maze
	if (maze->ballX != newX || maze->ballY != newY) {
		maze->ballX = newX;
		maze->ballY = newY;
		moves++;

		// if player has won the game
		if (maze->ballX == maze->goalX && maze->ballY == maze->goalY) {
			std::cout << "Maze completed in " << moves << " moves. Well done!" << std::endl;

			reset();
		}
	}
}
