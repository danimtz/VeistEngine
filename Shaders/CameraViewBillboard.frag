
#version 460

//layout (location = 0) in  vec3 inFragPos;
layout (location = 0) in  vec2 inUV;

layout (location = 0) out vec4 outFragColor;


layout(set = 1, binding = 0) uniform sampler2D iconTexture;



void main()
{

	vec4 color = texture(iconTexture, inUV);

	//Alpha test
	if(color.a < 0.5) {
		discard;
	}



	//gc_color = pow(env_color, vec3(0.4545)); 
	outFragColor = vec4(color.xyz, 1.0); 
	
}