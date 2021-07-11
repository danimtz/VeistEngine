#version 450


layout (location = 0) in  vec3 infragColor;

layout (location = 0) out vec4 outFragColor;

void main()
{
	outFragColor = vec4(infragColor, 1.0);

}