#version 400 core

uniform mat4 modelViewProjMat;

in vec3 vPosition;
out vec2 uv;

void main()
{
	gl_Position = modelViewProjMat * vec4(vPosition, 1.0);
	uv = vPosition.xy * 0.5f + 0.5f;
}