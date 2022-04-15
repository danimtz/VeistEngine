#version 450


//layout (location = 0) in vec2 inUV;

//layout (location = 0) out vec3 outPosition;

//layout (location = 0) out vec3 outNormal;
//layout (location = 1) out vec3 outTangent;
//layout (location = 2) out vec3 outBitangent;

layout (location = 0) out vec2 outUV;

/*
layout( push_constant ) uniform constants
{
	mat4 mModel;
	mat4 mNormal;
} push_constant;

layout(set = 0, binding = 0) uniform  cameraBuffer
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
} camera;
*/

void main() 
{
    outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	
    gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);
}