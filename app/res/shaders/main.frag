#version 410 core

in vec3 vertexColor;
in vec3 vertexNormal;
in vec3 worldPos;
in vec2 texCoord;

uniform vec3 camPos;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(vertexColor,1);
}