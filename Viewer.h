/**
 * Manage view matrix
 * Allow rotation about the Y axis
 */

#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "InputState.h"
#include "glm/glm.hpp"

class ObjectViewer {
private:
	glm::mat4 viewMtx;
	glm::vec3 initEye;
	glm::vec3 initAt;
	glm::vec3 initUp;

public:
	ObjectViewer( glm::vec3 eye );

	void update( InputState &input);
	float getCameraRotation();

	const glm::mat4 getViewMtx() const;
	void reset();
};

#endif
