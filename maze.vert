#version 330

// Position. 1 per vertex.
layout (location = 0) in vec3 a_vertex; 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 pos;

void main(void) {
	// pass object coordinates to frag shader
	pos = vec4(a_vertex, 1.0);

	// clip-space position
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
