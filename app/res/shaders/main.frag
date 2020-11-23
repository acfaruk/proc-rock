#version 330 core

in vec3 fColor;
in vec3 fNormal;
in vec3 fTangent;
in vec3 fBitangent;
in vec3 fPosition;
in vec2 fTexCoord;

uniform vec3 camPos;
uniform int lightCount;
uniform vec3[20] lightPos;
uniform vec3[20] lightColors;
uniform float[20] lightIntensities;
uniform vec3 ambientColor;
uniform bool enableParallax;
uniform float parallaxDepth;

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metalMap;
uniform sampler2D ambientOccMap;
uniform sampler2D displacementMap;

layout(location = 0) out vec4 fragColor;

#define M_PI 3.1415926535897932384626433832795

vec2 ParallaxMap(vec2 texCoords, vec3 viewDir)
{ 
    float heightScale = parallaxDepth;

    const float minLayers = 4;
    const float maxLayers = 32;
//    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));   // this crashes nvidia driver sometimes?
    float numLayers = 16;
    float layerDepth = 1.0 / numLayers;
    float layerDepthCurrent = 0.0;

    vec2 P = viewDir.xy * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    vec2  texCoordsCurrent = texCoords;
    float displaceValue = texture(displacementMap, texCoordsCurrent).r;
      
    while(layerDepthCurrent < displaceValue)
    {
        texCoordsCurrent -= deltaTexCoords;
        displaceValue = texture(displacementMap, texCoordsCurrent).r;  
        layerDepthCurrent += layerDepth;  
    }
//    return texCoordsCurrent;

    vec2 texCoordsPrevious = texCoordsCurrent + deltaTexCoords;

    float depthAfter = displaceValue - layerDepthCurrent;
    float depthBefore = texture(displacementMap, texCoordsPrevious).r - layerDepthCurrent + layerDepth;
 
    float weight = depthAfter / (depthAfter - depthBefore);
    vec2 finalTexCoords = texCoordsPrevious * weight + texCoordsCurrent * (1.0 - weight);
    return finalTexCoords;
}

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
    mat3 TBN = mat3(fTangent, fBitangent, fNormal);
	vec3 viewDir = normalize(camPos - fPosition);

    vec2 finalTexCoords = fTexCoord;
    if (enableParallax){
        finalTexCoords = ParallaxMap(fTexCoord, normalize(transpose(TBN) * viewDir));
    }

	vec3 color = pow(texture(albedo, finalTexCoords).rgb, vec3(2.2));
	float metallic = texture(metalMap, finalTexCoords).r;
	float roughness = texture(roughnessMap, finalTexCoords).r;
	float ambientOcc = texture(ambientOccMap, finalTexCoords).r;

	vec3 normal = normalize(texture(normalMap, finalTexCoords).rgb * 2.0 - 1.0);
    normal = normalize(TBN * normal);


	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, color, metallic);

	vec3 lightOut = vec3(0.0);
	for (int i = 0; i < lightCount; i++) {
		vec3 lightDir = normalize(lightPos[i] - fPosition);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		float distance = length(lightPos[i] - fPosition);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * lightIntensities[i] * attenuation;

		float NDF = DistributionGGX(normal, halfwayDir, roughness);
		float G = GeometrySmith(normal, viewDir, lightDir, roughness);
		vec3 F = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denumerator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
		vec3 specular = numerator / denumerator;

		vec3 kD = vec3(1.0) - F;
		kD *= 1.0 - metallic;

		float n_dot_l = max(dot(normal, lightDir), 0.0);
		lightOut +=  (kD * color / M_PI + specular) * radiance * n_dot_l;
	}

	// ambient
	vec3 ambient = ambientColor * color * ambientOcc;

	vec3 resultColor = ambient + lightOut;

	resultColor = resultColor / (resultColor + vec3(1.0));
	resultColor = pow(resultColor, vec3(1.0 / 2.2));

	fragColor = vec4(resultColor, 1.0);
}