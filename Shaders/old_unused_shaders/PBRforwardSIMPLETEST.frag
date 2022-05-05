#version 450 

#include "utils/brdf.glsl"
#include "utils/utils.glsl"
#define MAX_DIR_LIGHTS 4




layout (location = 0) in  vec3 inFragPos;

layout (location = 1) in  vec3 inNormal;
layout (location = 2) in  vec3 inTangent;
layout (location = 3) in  vec3 inBitangent;

layout (location = 4) in  vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  sceneInfo{
	float dir_light_count;
	float point_light_count;
}scene_info;

layout(set = 0, binding = 1) uniform  cameraBuffer
{
	Camera camera_data;
};

layout(set = 0, binding = 2) uniform  directionalLights{
	DirLights dir_lights[MAX_DIR_LIGHTS];
};

layout(std140, set = 0, binding = 3) readonly buffer pointLights{
	PointLights point_lights[];
};


layout(set = 0, binding = 4) uniform samplerCube inIrradianceMap;
layout(set = 0, binding = 5) uniform samplerCube inPrefilterMap;
layout(set = 0, binding = 6) uniform sampler2D inBRDF_LUT;

layout(set = 1, binding = 0) uniform sampler2D inAlbedo;
layout(set = 1, binding = 1) uniform sampler2D inNormalTex;
layout(set = 1, binding = 2) uniform sampler2D inOccRoughMetal;
layout(set = 1, binding = 3) uniform sampler2D inEmmissive;






vec3 calculateDiffuseSpec(vec3 N, vec3 L, vec3 V, vec3 albedo)
{

	vec3 H = normalize(L + V);

	float diffuse = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), 128.0);

	vec3 color = (diffuse * albedo) + (spec * albedo);

	return color;
}





float calcAttenuation(float radius, vec3 light_pos, vec3 fragPos){
	float dist = length(light_pos-fragPos);
	float k_linear = 2.0/radius;
	float k_quadratic = 1.0/(radius*radius);
	return 1.0/(1.0 + k_linear*dist + k_quadratic*dist*dist); //TODO implement cutoff	
}






void main()
{
	//Normal mapping
	mat3 mTBN = mat3(normalize(inTangent), normalize(inBitangent), normalize(inNormal));
	vec3 tex_normal = normalize(texture(inNormalTex, inUV).xyz * 2.0 - 1.0);
	
	//
	vec3 N = normalize(mTBN * tex_normal);
	vec3 V = -normalize(inFragPos);
	vec3 worldR = vec3(camera_data.mInvV * vec4(reflect(-V, N), 1.0));   

	vec3 albedo = texture(inAlbedo, inUV).xyz; 
	float roughness = texture(inOccRoughMetal, inUV).y;
	float metallic = texture(inOccRoughMetal, inUV).z;
	float occlusion = texture(inOccRoughMetal, inUV).x;
	vec3 emmissive = texture(inEmmissive, inUV).xyz;


	//Directional lights
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < scene_info.dir_light_count; i++)
	{
		vec3 L = dir_lights[i].direction;
		vec3 radiance = dir_lights[i].colour * dir_lights[i].intensity;
		Lo += radiance * calculateDiffuseSpec(N, L, V, albedo);

	}

	//Point lights
	for (int i = 0; i < scene_info.point_light_count; i++)
	{
		vec3 L = normalize(point_lights[i].position - inFragPos);
		float attenuation = calcAttenuation(point_lights[i].radius, point_lights[i].position, inFragPos);
		
		vec3 radiance = point_lights[i].colour * attenuation * point_lights[i].intensity;
		Lo += radiance * calculateDiffuseSpec(N, L, V, albedo);

	}

	


	vec3 color = Lo ;//+ ambient + emmissive;
	//color = color / (color + vec3(1.0)); //reindhardt tone map
	color = pow(color, vec3(0.4545)); //GAMMA CORRECTION
	vec3 final_color = clamp(color, 0.0, 1.0);

	outFragColor = vec4(final_color, 1.0);
}