/**
 * Maze game: Move ball to goal location.
 *
 * Based on model-view example from lectures
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GameManager.h"
#include "InputState.h"
#include "Viewer.h"
#include "Maze.h"
#include "Shader.hpp"

// Window created with GLFW
int winX = 640;
int winY = 480;
GLFWwindow *window;

// The maze and related objects.
float mazeWidth = 10.0f;
Maze *maze;

GameManager *gameManager;

ObjectViewer *camera;

// Data structure storing mouse input info
InputState Input;

// Shader program
unsigned int programID;
int viewHandle;

// GLFW callback: Keyboard game controls
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch(key) 
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_RIGHT:
			case GLFW_KEY_UP:
			case GLFW_KEY_LEFT:
			case GLFW_KEY_DOWN:
				gameManager->moveBall(key, camera->getCameraRotation());
				break;
			default:
				break;
			}
	}
}	

// GLFW callback: Store mouse position info
void mouse_pos_callback(GLFWwindow* window, double x, double y) {
	Input.update((float)x);
}

// GLFW callback: Store mouse button info
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Input.lMousePressed = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		Input.lMousePressed = false;
	}
}

/* 
 * Set the projection matrix. Takes into account window aspect ratio, so called
 * when the window is resized.
 */
void setProjection() {
	glm::mat4 projection;
	
	projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 1.0f, 30.0f );

	// Load it to the shader program
	int projHandle = glGetUniformLocation(programID, "projection");
	if (projHandle == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	// upload our projection matrix to shader
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
}

// GLFW callback: Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y ) {
	winX = x;
	winY = y;
	setProjection();
	glViewport( 0, 0, x, y );
}

// GLFW callback: Error. Simply prints error message to stderr.
static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

/**
 * Render frame
 */
void render() {
	// Set up the scene and the camera
	setProjection();

	// Update the camera, and draw the scene.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Store user input to update view matrix in camera
	camera->update(Input);

	// First load the viewing matrix from camera (controlled by mouse)
	glm::mat4 viewMatrix;
	viewMatrix = camera->getViewMtx();
	
	// Load it to the shader program
	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );

	// Draw the maze
	maze->render();
}

/**
 * Check that command line args are valid
 * @param argc Number of command line args
 * @param argv Array of command line args
 * @return 0 if command line args are valid, 1 otherwise
 */
int checkCmdLineArgs(int argc, char ** argv) {
	// correct number of args
	if (argc != 2) {
		printf("Usage: maze path/to/mazeFile\n");
		return 1;
	}

	// maze file is readable
	std::ifstream mazeFile(argv[1]);
	if (!mazeFile.good()) {
		printf("File is not readable: %s\n", argv[1]);
		return 1;
	}

	return 0;
}

/**
 * Create maze from file
 * If maze file is supplied and the maze has valid size, instantiate a Maze
 * @param filePath Path to file that defines maze
 * @return 0 if maze can be created, 1 otherwise
 */
int createMaze(char* filePath) {
	// open stream to file
	std::ifstream fileStream(filePath);
	if (fileStream == NULL) {
		printf("Couldn't open file stream to: %s\n", filePath);
		return 1;
	}

	// check size of maze
	std::string mazeLine;
	int gridSize;
	getline(fileStream, mazeLine);
	std::stringstream(mazeLine) >> gridSize;
	if (gridSize < 2) {
		printf("Please enter a maze size >= 2.\n");
		return 1;
	}

	// read maze specification from file
	std::vector<std::string> grid;
	for (int row = 0; row < gridSize; row++) {
		getline(fileStream, mazeLine);
		grid.push_back(mazeLine);
	}

	maze = new Maze(grid, mazeWidth, programID);
	gameManager = new GameManager(maze);

	return 0;
}


/**
 * Sets up the GLFW window before rendering starts
 */
void setupGLFWWindow() {
	if (!glfwInit()) {
		exit(1);
	}

	// Specify that we want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window and OpenGL context
	window = glfwCreateWindow(winX, winY, "Maze", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Define callback functions
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);
}

/**
 * Load shader required for the maze, and find uniform variables
 */
void setupShader() {
	// Set up the shaders we are to use. 0 indicates error.
	programID = LoadShaders("maze.vert", "maze.frag");
	if (programID == 0) {
		exit(1);
	}

	// Get handle to view matrix to control camera
	viewHandle = glGetUniformLocation(programID, "view");
	if (viewHandle == -1) {
		std::cout << "Uniform: view is not an active uniform label\n";
	}
}

/**
 * Sets up OpenGL state, GLSL Shaders, GLFW window and function call backs
 * Instantiate maze and camera, then do rendering loop
 * @param argc Arg count - Expecting 2
 * @param argv Args - Expecting path to maze file
 */
int main (int argc, char **argv) {
	if (checkCmdLineArgs(argc, argv) == 1) {
		return 0;
	}

	setupGLFWWindow();
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	setupShader();

	// Create maze from file
	if (createMaze(argv[1]) == 1) {
		return 0;
	}

	// Create camera that can be controlled by user
	glm::vec3 initialCameraPos(0.0f, 1.05f*mazeWidth, 1.05f*mazeWidth);
	camera = new ObjectViewer(initialCameraPos);

	// Set OpenGL state we need for this application.
	glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup    
	glfwDestroyWindow(window);
	glfwTerminate();

	delete camera;
	delete gameManager;
	delete maze;
	
	return 0;
}
