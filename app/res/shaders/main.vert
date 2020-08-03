#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec2 vTexCoords;

out vec3 vertexColor;
out vec3 vertexNormal;
out vec3 fragPos;
out vec3 worldPos;
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;
uniform bool vertexColored;

void main(){
	texCoord = vTexCoords;
	worldPos = vec3(modelMatrix * vec4(vPosition, 1));
	if (vertexColored){
		vertexColor = vColor;
	}else{
		vertexColor = vec3(1, 1, 1);
	}
	vertexColor = vColor;
	vertexNormal = vNormal;
	texCoord = vTexCoords;
	fragPos = vPosition;
	gl_Position = mvpMatrix * vec4(vPosition, 1);
}
