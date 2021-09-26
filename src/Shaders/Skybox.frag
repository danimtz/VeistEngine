

#version 450


layout (location = 0) in  vec3 inFragPos;

//layout (location = 1) in  vec3 inNormal;
//layout (location = 2) in  vec3 inTangent;
//layout (location = 3) in  vec3 inBitangent;
//layout (location = 4) in  vec2 inUV;

layout (location = 0) out vec4 outFragColor;



layout(set = 0, binding = 0) uniform samplerCube skybox;



void main()
{
	outFragColor = texture(skybox, inFragPos);
	
}