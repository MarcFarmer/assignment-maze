#include <math.h>

#include "Viewer.h"
#include "glm/gtc/matrix_transform.hpp"

#define DEG2RAD(x) ((x)*M_PI/180.0) 

// How fast to rotate the camera
const float rotateSpeed = 0.6f;

// Total rotation
float cumulativeRotation = 0.0f;

/*
 * Initialise viewer relative to maze
 * @param eye Initial viewer position
 */
ObjectViewer::ObjectViewer( glm::vec3 eye ) {
	initEye = eye;
	reset();
}

/**
 * Assumes the current matrix has been calculated (usually in 
 * update() or reset())
 * @return View matrix
 */
const glm::mat4 ObjectViewer::getViewMtx() const {
	return viewMtx;
}

/**
 * Resets the view matrix to the value the camera was 
 * initialised with. Assumes looking at the origin.
 */
void ObjectViewer::reset() {
	glm::vec3 at(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	viewMtx = glm::lookAt(initEye, at, up);
}

/**
 * Check state of user input, use it to update view matrix
 * Allow rotation about the y axis from horizontal mouse movement
 */
void ObjectViewer::update(InputState &input) {
	float xRot;
	input.readDeltaAndReset( &xRot );
	
	// Rotate about the eye's y axis.
	if ( input.lMousePressed )
	{
		glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);

		float rotation = rotateSpeed * DEG2RAD(xRot);
		cumulativeRotation += rotation;

		viewMtx = glm::rotate(viewMtx, rotation, yAxis);
	}
}

/**
 * Get the amount of anticlockwise rotation from the initial camera position
 * Rotation is measured in radians from 0 to 2*PI
 * @return Rotation from initial camera position
 */
float ObjectViewer::getCameraRotation() {
	float cameraRotation = fmod(cumulativeRotation, 2*M_PI);

	// convert negative rotation to positive
	if (cameraRotation < 0) {
		cameraRotation = 2*M_PI - (-cameraRotation);
	}

	return cameraRotation;
}
