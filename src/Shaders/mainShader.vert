#version 450


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec2 inUV;


layout (location = 0) out vec3 outPosition;

layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outTangent;
layout (location = 3) out vec3 outBitangent;

layout (location = 4) out vec2 outUV;

layout( push_constant ) uniform constants
{
	mat4 Mmat;
	mat4 Nmat;
} push_constant;

layout(set = 0, binding = 1) uniform  cameraBuffer
{
	mat4 Vmat;
	mat4 Pmat;
	mat4 VPmat;
} camera_data;





void main()
{
	
	//output the position of each vertex
	gl_Position = (camera_data.VPmat*push_constant.Mmat)*vec4(inPosition, 1.0f);
	outPosition = vec3((camera_data.Vmat*push_constant.Mmat)*vec4(inPosition, 1.0f));
	
	outNormal = vec3(push_constant.Nmat * vec4(inNormal, 1.0f));
	outTangent = vec3(push_constant.Nmat * vec4(inTangent, 1.0f));
	outBitangent = normalize(cross(outTangent, outNormal));

	
	outUV = inUV;
}