// One megashader for floor, blocks, goal and ball due to bugs with multiple shaders.

#version 330

in vec4 pos;

// The final colour we will see at this location on screen
out vec4 fragColour;

// Which part of the maze we are rendering
uniform int renderStage;

// Radius of goal and ball
uniform float sphereRadius;
// "Radius" of square on top of blocks and floor tiles
uniform float squareRadius;

#define RENDER_STAGE_FLOOR 0
#define RENDER_STAGE_BLOCKS 1
#define RENDER_STAGE_GOAL 2
#define RENDER_STAGE_BALL 3

/*
 * Determine fragment colour by the xz position on the cube
 * Use squares to make a 'tiled' appearance to make structure of the maze clear
 * @param tileColour Colour in the square on top of the cube
 * @param sideColour Colour outside of the square on top of the cube
 * @return Colour for this part of the cube
 */
vec4 cubeColour(in vec4 tileColour, in vec4 sideColour) {
	if (pos.x < squareRadius && pos.x > -squareRadius && 
		pos.z < squareRadius && pos.z > -squareRadius) {
		return tileColour;
	} else {
		return sideColour;
	}
}

/*
 * Determine fragment colour by the y position of this fragment in the sphere
 * Use a gradient for a basic 3D appearance
 * @param lowerColour Colour at the bottom of the sphere
 * @param upperColour Colour at the top of the sphere
 * @return Colour interpolated between lower and upper
 */
vec4 sphereColour(in vec4 lowerColour, in vec4 upperColour) {
	// move y position from [-sphereRadius, sphereRadius] to [0, 1]
	float lerpValue = (pos.y + sphereRadius) / (2*sphereRadius);

	return mix(lowerColour, upperColour, lerpValue);
}

void main(void) {
	vec4 darkPurple = vec4(0.2, 0.0, 0.6, 1.0);

	// different rendering style for each of the 4 components of the maze
	switch(renderStage) {
		case RENDER_STAGE_FLOOR:
			vec4 yellow = vec4(1.0f, 0.75f, 0.0f, 1.0f);
			fragColour = cubeColour(yellow, darkPurple);
			break;

		case RENDER_STAGE_BLOCKS:
			vec4 purple = vec4(0.67f, 0.04f, 0.83f, 1.0f);
			fragColour = cubeColour(purple, darkPurple);
			break;

		case RENDER_STAGE_GOAL:
			vec4 green = vec4(0.2f, 0.8f, 0.2f, 1.0f);
			vec4 darkGreen = vec4(0.0f, 0.5f, 0.0f, 1.0f);
			fragColour = sphereColour(darkGreen, green);
			break;

		case RENDER_STAGE_BALL:
			vec4 red = vec4(1.0f, 0.0f, 0.1f, 1.0f);
			vec4 darkRed = vec4(0.5f, 0.0f, 0.0f, 0.1f);
			fragColour = sphereColour(darkRed, red);
			break;

		default:
			vec4 white = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			fragColour = white;
			break;
	}
}
