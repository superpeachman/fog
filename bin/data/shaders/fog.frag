#version 330
precision mediump float;

uniform sampler2D depthTexture;
uniform sampler2D noiseTexture;

uniform vec2 offset;
uniform float distLength;
uniform float fogHighlightX;
uniform float fogHighlightY;
uniform mat4 inv;

uniform vec3 eyeDirection;
uniform vec3 lightDirection;
uniform vec4 ambientColor;

in vec4 vPosition;
in vec3 vNormal;
in vec4 vColor;
in vec2 vTexCoord;

out vec4 outputColor;

void main(void){
    vec3 invLight = normalize(inv * vec4(lightDirection, 0.0)).xyz;
    vec3 invEye = normalize(inv * vec4(eyeDirection, 0.0)).xyz;
    vec3 halfLE = normalize(invLight + invEye);
    float diffuse = clamp(dot(vNormal, invLight), 0.0, 1.0);
    float specular = pow(clamp(dot(vNormal, halfLE), 0.0, 1.0), 50.0);

	vec4 noiseColor = texture(noiseTexture, vTexCoord + offset);

	// I don't know why but this is not well work in .Vert
    vec3 texVPosition = (vPosition.xyz / vPosition.w + 1.0) * 0.5;
    float depth = texture(depthTexture, texVPosition.xy).r;

	float alpha = 1.0 - clamp(length(vec2(fogHighlightX, fogHighlightY) - vTexCoord) * 2.0, 0.0, 1.0);

	float distance = abs(depth - texVPosition.z);
	if(distLength >= distance){
		float d = distance / distLength;
		alpha *= d;
	}

	// outputColor = vec4(
	// 	vColor.rgb * vec3(diffuse) + vec3(specular) + ambientColor.rgb,
	// 	noiseColor * alpha
	// 	);
	outputColor = vec4(
		vColor.rgb,
		noiseColor * alpha
		);
}