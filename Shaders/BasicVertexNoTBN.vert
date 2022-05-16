#version 460

#include "utils/utils.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inTangent;
layout (location = 3) in vec2 inUV;


layout (location = 0) out vec3 outPosition;


layout(set = 0, binding = 0) uniform  cameraBuffer
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
} camera;

layout(std140, set = 0, binding = 1) readonly buffer objectMatrices
{
	ObjectMatrices object_matrices[];
};



void main()
{
	//output the position of each vertex
	gl_Position = (camera.mVP*object_matrices[gl_BaseInstance].mModel)*vec4(inPosition, 1.0f);

}