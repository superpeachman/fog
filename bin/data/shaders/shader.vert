#version 330
precision mediump float;

uniform mat4 model;
uniform mat4 modelView;
uniform mat4 mvp;

in vec3 position;
in vec3 normal;
in vec4 color;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;

void main(void){
	vPosition = (model * vec4(position, 1.0)).xyz;
	// vPosition = mvp * vec4(position, 1.0);

	vNormal = normal;
	vColor = color;

	gl_Position = mvp * vec4(position, 1.0);
}
