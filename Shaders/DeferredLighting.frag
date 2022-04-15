#version 450

#define MAX_DIR_LIGHTS 4

struct DirLights{
	vec3 direction;
	float intensity;
	vec3 colour;
	uint padding;//TODO REPLACE WITH PADDING.THIS IS DEPRECATED
};

struct PointLights{
	vec3 position;
	float intensity;
	vec3 colour;
	float radius;
};


layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  sceneInfo{
	float dir_light_count;
	float point_light_count;
}scene_info;


layout(set = 0, binding = 1) uniform  cameraBuffer
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
} camera_data;


layout(set = 0, binding = 2) uniform  directionalLights{
	DirLights dir_lights[MAX_DIR_LIGHTS];
};


layout(std140, set = 0, binding = 3) readonly buffer pointLights{
	PointLights point_lights[];
};

layout(set = 0, binding = 4) uniform samplerCube inIrradianceMap;
layout(set = 0, binding = 5) uniform samplerCube inPrefilterMap;
layout(set = 0, binding = 6) uniform sampler2D inBRDF_LUT;

layout (set = 0, binding = 7) uniform sampler2D inAlbedoMetallic;
layout (set = 0, binding = 8) uniform sampler2D inNormalRoughness;
layout (set = 0, binding = 9) uniform sampler2D inEmmissiveOcclusion;


void main()
{

    

	outFragColor = vec4(1.0, 1.0, 0.5, 1.0);
}