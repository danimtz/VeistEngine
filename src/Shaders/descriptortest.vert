#version 450


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;

layout( push_constant ) uniform constants
{
	mat4 Mmat;
} push_constant;

layout(set = 0, binding = 0) uniform  cameraBuffer{
	mat4 Vmat;
	mat4 Pmat;
	mat4 VPmat;
	mat4 Nmat;
} camera_data;





void main()
{
	
	//output the position of each vertex
	gl_Position = (camera_data.VPmat*push_constant.Mmat)*vec4(position, 1.0f);
	//outColor = vec3(abs(normal.x), abs(normal.y), abs(normal.z));
	outNormal = vec3(camera_data.Nmat * vec4(normal, 1.0f));
	outColor = vec3(0.5, 0.8, 0.4);
}