

#version 460

layout(early_fragment_tests) in;
layout (location = 0) in  vec3 inFragPos;

//layout (location = 1) in  vec3 inNormal;
//layout (location = 2) in  vec3 inTangent;
//layout (location = 3) in  vec3 inBitangent;
//layout (location = 4) in  vec2 inUV;

layout (location = 0) out vec4 outFragColor;



layout(set = 0, binding = 0) uniform samplerCube skybox;



void main()
{

	vec3 env_color =  texture(skybox, inFragPos).rgb;
	//env_color = env_color/(env_color + vec3(1.0));//tonemap
	env_color = pow(env_color, vec3(0.4545)); 
	outFragColor = vec4(env_color, 1.0); 
	
}