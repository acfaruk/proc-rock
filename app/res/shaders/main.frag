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

#define M_PI 3.1415926535897932384626433832795

float DistributionGGX(vec3 normal, vec3 halfwayDir, float roughness)
{
    float a = roughness * roughness;
    float aSquared = a * a;
    float n_dot_h = max(dot(normal, halfwayDir), 0.0);
    float n_dot_h_squared = n_dot_h * n_dot_h;

    float numerator  = aSquared;
    float denumerator = (n_dot_h_squared * (aSquared - 1.0) + 1.0);
    denumerator = M_PI * denumerator * denumerator;

    return numerator / denumerator;
}
float GeometrySchlickGGX(float n_dot_v, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float numerator   = n_dot_v;
    float denumerator = n_dot_v * (1.0 - k) + k;

    return numerator / denumerator;
}
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float n_dot_v = max(dot(normal, viewDir), 0.0);
    float n_dot_l = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(n_dot_v, roughness);
    float ggx1 = GeometrySchlickGGX(n_dot_l, roughness);

    return ggx1 * ggx2;
}
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 color = texture(albedo, texCoord).xyz;
	float metallic = 0.0;
	float roughness = 1.0;
	float ambientOcc = 1.0;

	vec3 normal = normalize(texture(normalMap, texCoord).rgb * 2.0 - 1.0);
	vec3 viewDir = normalize(tangentSpace.camPos - tangentSpace.fragPos);

	vec3 F0 = mix(vec3(0.04), color, metallic);

	vec3 lightDir = normalize(tangentSpace.lightPos - tangentSpace.fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float distance = length(tangentSpace.lightPos - tangentSpace.fragPos);
//	float attenuation = 1.0 / (distance * distance);
//	vec3 radiance = lightColor * attenuation;

	float NDF = DistributionGGX(normal, halfwayDir, roughness);
	float G = GeometrySmith(normal, viewDir, lightDir, roughness);
	vec3 F = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denumerator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
	vec3 specular = numerator / denumerator;

	vec3 kD = vec3(1.0) - F;
	kD *= 1.0 - metallic;

	float n_dot_l = max(dot(normal, lightDir), 0.0);

	vec3 lightOut =  (kD * color / M_PI + specular) /** attenuation */* n_dot_l;


	// ambient
	vec3 ambient = ambientColor * color * ambientOcc;

	vec3 resultColor = ambient + lightOut;

	resultColor = resultColor / (resultColor + vec3(1.0));
	resultColor = pow(resultColor, vec3(1.0 / 2.2));

	fragColor = vec4(resultColor, 1.0);
}