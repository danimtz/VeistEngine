#version 450


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;

layout( push_constant ) uniform constants
{
	mat4 MVPmat;
	mat4 Nmat;
} push_constant;

void main()
{
	
	//output the position of each vertex
	gl_Position = push_constant.MVPmat*vec4(position, 1.0f);
	//outColor = vec3(abs(normal.x), abs(normal.y), abs(normal.z));
	outNormal = vec3(push_constant.Nmat * vec4(normal, 1.0f));
	outColor = vec3(0.5, 0.8, 0.4);
}