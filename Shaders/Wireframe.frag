#version 460 

#include "utils/brdf.glsl"
#include "utils/utils.glsl"
#define MAX_DIR_LIGHTS 4




layout (location = 0) in  vec3 inFragPos;


layout (location = 0) out vec4 outFragColor;




//Material unused
layout(set = 1, binding = 0) uniform sampler2D inAlbedo;
layout(set = 1, binding = 1) uniform sampler2D inNormalTex;
layout(set = 1, binding = 2) uniform sampler2D inOccRoughMetal;


void main()
{

	outFragColor = vec4(0.0, 0.0, 0.0, 1.0);

}