#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec2 vTexCoords;

out vec3 fColor;
out vec3 fNormal;
out vec3 fTangent;
out vec3 fBinormal;
out vec3 fPosition;
out vec2 fTexCoord;

uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;
uniform bool vertexColored;
uniform vec3 camPos;

uniform int lightCount;
uniform vec3[20] lightPos;

void main(){
	fPosition = vec3(modelMatrix * vec4(vPosition, 1));

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	
	fTexCoord = vTexCoords;
	fNormal = normalize(normalMatrix * vNormal);
	fTangent = normalize(normalMatrix * vTangent);
	fTangent = normalize(fTangent - dot(fTangent, fNormal) * fNormal);
//	fBinormal = normalize(normalMatrix*cross(vNormal, vTangent));
	fBinormal = cross(fNormal, fTangent);

	if (vertexColored){
		fColor = vColor;
	}else{
		fColor = vec3(1, 1, 1);
	}
	gl_Position = mvpMatrix * vec4(vPosition, 1);
}
