#version 330
precision mediump float;

uniform mat4 mvp;

in vec3 position;
in vec4 color;

out vec4 vColor;

void main(void) {
	vColor = color;

	gl_Position = mvp * vec4(position, 1.0);
}
