#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec2 vTexCoords;

out vec3 vertexColor;
out vec3 vertexNormal;
out vec3 vertexTangent;
out vec3 fragPos;
out vec2 texCoord;

out TangentSpace {
	vec3[20] lightPos;
	vec3 camPos;
	vec3 fragPos;
} tangentSpace;

uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;
uniform bool vertexColored;
uniform vec3 camPos;

uniform int lightCount;
uniform vec3[20] lightPos;

void main(){

	vec3 vBitangent = cross(vNormal, vTangent);
	fragPos = vec3(modelMatrix * vec4(vPosition, 1));

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 T = normalize(normalMatrix * vTangent);
	vec3 N = normalize(normalMatrix * vNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));

	for (int i = 0; i < lightCount; i++) {
		tangentSpace.lightPos[i] = TBN * lightPos[i];
	}

	tangentSpace.camPos = TBN * camPos;
	tangentSpace.fragPos = TBN * fragPos;
	
	if (vertexColored){
		vertexColor = vColor;
	}else{
		vertexColor = vec3(1, 1, 1);
	}
	vertexNormal = vNormal;
	vertexTangent = vTangent;
	texCoord = vTexCoords;
	gl_Position = mvpMatrix * vec4(vPosition, 1);
}
