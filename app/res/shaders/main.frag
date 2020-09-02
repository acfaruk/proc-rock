#version 330 core

in vec3 vertexColor;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 fragPos;
in vec2 texCoord;

in TangentSpace {
	vec3 lightPos;
	vec3 camPos;
	vec3 fragPos;
} tangentSpace;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;

uniform sampler2D albedo;
uniform sampler2D normalMap;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 color = texture(albedo, texCoord).xyz;

	// ambient
	vec3 ambient = ambientColor * lightColor * color;

	// diffuse
	vec3 lightDir = normalize(tangentSpace.lightPos - tangentSpace.fragPos);
	vec3 normal = normalize(texture(normalMap, texCoord).rgb * 2.0 - 1.0);

	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor * color;

	// specular
	vec3 viewDir = normalize(tangentSpace.camPos - tangentSpace.fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

	vec3 specular = lightColor * spec;

	fragColor = vec4(ambient + diffuse + specular, 1.0);
}