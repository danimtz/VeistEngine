#version 460

#include "utils/utils.glsl"



layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;


layout( push_constant ) uniform constants
{
	int target_select;
}p_constant;


layout (set = 0, binding = 0) uniform sampler2D inAlbedo;
layout (set = 0, binding = 1) uniform sampler2D inNormal;
layout (set = 0, binding = 2) uniform sampler2D inOccRoughMetal;
//layout (set = 0, binding = 10) uniform sampler2D inEmmissive;
layout (set = 0, binding = 3) uniform sampler2D inDepth;


void main()
{
	float col;
	vec3 albedo;
	switch (p_constant.target_select){

		case 1:
			albedo = texture(inAlbedo, inUV).xyz;
			albedo = pow(albedo, vec3(0.4545));
			outFragColor = vec4(albedo, 1.0);
			break;

		case 2:
			outFragColor = vec4(texture(inNormal, inUV).xyz, 1.0);
			break;

		case 3:
			col = texture(inOccRoughMetal, inUV).z;
			outFragColor = vec4(col,col,col,1.0);
			break;

		case 4:
			col = texture(inOccRoughMetal, inUV).y;
			outFragColor = vec4(col,col,col,1.0);
			break;

		case 5:
			col = texture(inDepth, inUV).x;
			outFragColor = vec4(col,col,col,1.0);
			break;
		
		case 6:
			col = texture(inOccRoughMetal, inUV).x;
			outFragColor = vec4(col,col,col,1.0);
			break;

		default:
			discard;
	}

}