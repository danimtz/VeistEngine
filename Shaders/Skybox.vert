#version 460


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 padding1;
layout (location = 2) in vec4 padding2;
layout (location = 3) in vec2 padding3;

layout (location = 0) out vec3 outPosition;


layout( push_constant ) uniform constants
{
	mat4 mV;
	mat4 mP;
} push_constant;


void main()
{
	//output the position of each vertex
	gl_Position = ((push_constant.mP*push_constant.mV)*vec4(inPosition, 1.0f)).xyww;
	outPosition = inPosition;

}