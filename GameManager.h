/*
 * Move the ball and check if player has won.
 */

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Maze.h"

#include <string>
#include <vector>

class GameManager {
public:
	GameManager(Maze* maze);
	void moveBall(int key, float cameraRotation);

private:
	Maze* maze;
	int moves;

	void reset();
	int findCameraDirection(float rotation);
	int findGridDirection(int key, float cameraRotation);
};

#endif
