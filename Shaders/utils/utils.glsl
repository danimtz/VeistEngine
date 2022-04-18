#ifndef UTILS_GLSL
#define UTILS_GLSL



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


struct Camera
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
	mat4 mInvP;
};
#endif