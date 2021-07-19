#version 450


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 outColor;

layout( push_constant ) uniform constants
{
	mat4 MVPmat;
} push_constant;

void main()
{
	
	//output the position of each vertex
	gl_Position = push_constant.MVPmat*vec4(position, 1.0f);
	outColor = vec3(abs(normal.x), abs(normal.y), abs(normal.z));
}