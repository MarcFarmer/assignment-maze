/**
 * A set of variables that records horizontal (x axis) mouse activity.
 * You can update the variables yourself or use the functions.
*/

#ifndef _INPUTSTATE_H_
#define _INPUTSTATE_H_

struct InputState {
	InputState(): lMousePressed(false),
				  prevX(0),
				  deltaX(0) {}

	// Is the mouse button currently being held down?
	bool lMousePressed;

	// Last known position of the cursor
	float prevX;

	// Accumulated change in cursor position. 
	float deltaX;

	// Update cursor variables based on new position x
	void update(float x) {
		float xDiff = x - prevX;
		deltaX += xDiff;
		prevX = x;
	}

	// Read off the accumulated motion and reset it
	void readDeltaAndReset(float *x) {
		*x = deltaX;
		deltaX = 0;
	}
};

#endif // _INPUTSTATE_H_
