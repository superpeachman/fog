#version 330
precision mediump float;

uniform mat4 model;
uniform mat4 mvp;

in vec3 position;
in vec4 color;
in vec3 normal;
in vec2 texcoord;

out vec4 vPosition;
out vec3 vNormal;
out vec4 vColor;
out vec2 vTexCoord;
out vec3 vTexProjCoord;

void main(void){
	vPosition = mvp * vec4(position, 1.0);
	vNormal = normal;
	vColor = color;
	vTexCoord = texcoord;

	gl_Position = vPosition;
}