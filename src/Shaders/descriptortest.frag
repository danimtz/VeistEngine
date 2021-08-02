#version 450


layout (location = 0) in  vec3 infragColor;
layout (location = 1) in  vec3 normal;

layout (location = 0) out vec4 outFragColor;




void main()
{

	vec3 light = normalize(vec3(0.4, 0.7, 1.0) - vec3(0.0, 0.0, 0.0));

	vec3 color = max(0.0, dot(normal, light)) * infragColor;

	outFragColor = vec4(color, 1.0);

}