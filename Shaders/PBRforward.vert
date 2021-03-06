#version 460

#include "utils/utils.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inTangent;
layout (location = 3) in vec2 inUV;


layout (location = 0) out vec3 outPosition;

layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outTangent;
layout (location = 3) out vec3 outBitangent;

layout (location = 4) out vec2 outUV;



layout(set = 0, binding = 1) uniform  cameraBuffer
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
} camera;

layout(std140, set = 0, binding = 4) readonly buffer objectMatrices
{
	ObjectMatrices object_matrices[];
};

/*
layout( push_constant ) uniform constants
{
	int object_id
} push_constant;
*/



void main()
{
	
	//output the position of each vertex
	gl_Position = (camera.mVP*object_matrices[gl_BaseInstance].mModel)*vec4(inPosition, 1.0f);
	outPosition = vec3((camera.mV*object_matrices[gl_BaseInstance].mModel)*vec4(inPosition, 1.0f));
	
	mat3 Nmat = mat3(object_matrices[gl_BaseInstance].mNormal); //mN is already a 3x3 extended to a 4x4 matrix but this is cleaner

	outNormal = normalize(Nmat * inNormal);
	outTangent = normalize(Nmat * inTangent.xyz);
	outBitangent = normalize(cross(outNormal, outTangent) * inTangent.w);

	outUV = inUV;

}