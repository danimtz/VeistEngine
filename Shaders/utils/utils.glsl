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


struct ObjectMatrices
{
	mat4 mModel;
	mat4 mNormal;
};


struct ShadowMapData
{
	mat4 mLightSpaceMatrix;
	int map_size;
};


struct Camera
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
	mat4 mInvP;
};




vec3 project(vec3 pos, mat4 matrix)
{

	vec4 point = (matrix * vec4(pos, 1.0)); 
	point /= point.w;

	point.xy = point.xy * 0.5 + vec2(0.5); //convert to 0 to 1 range
	//point.z;
	return point.xyz;
}


vec3 positionFromDepth(vec2 uv, float depth, mat4 inv_proj_mat)
{
	vec4 ndc_pos = vec4(uv*2.0-1.0, depth, 1.0);
	vec4 view_space_pos = inv_proj_mat * ndc_pos;
	//perpesctive divide
	vec4 pos = view_space_pos/view_space_pos.w;
	return pos.xyz;
}


#endif