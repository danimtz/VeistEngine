#version 450

#define MAX_DIR_LIGHTS 4
struct DirLights{
	vec3 direction;
	float intensity;
	vec3 colour;
	uint light_count;//padding/array length
};


layout (location = 0) in  vec3 infragColor;
layout (location = 1) in  vec3 normal;
layout (location = 2) in  vec3 position;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  directionalLights{
	DirLights dir_lights[MAX_DIR_LIGHTS];
};



void main()
{

	vec3 total_light = vec3(0.0);

	float kd = 0.8;//should be from material
	float ks = 0.3;

	for(int i = 0; i < dir_lights[0].light_count; i++){

		vec3 light_dir = dir_lights[i].direction;
		vec3 light_colour = dir_lights[i].colour;
		float intensity = dir_lights[i].intensity;
		float diffuse = max(0.0, dot(normal, light_dir));

		//specular TODO
		vec3 h = normalize(light_dir + position);
 		float spec = pow(max(0.0, dot(normal, h)), 128.0);//128 from material too 

		total_light +=  light_colour * (diffuse*kd + spec*ks)  * intensity;

	}
	
	vec3 color =  total_light * infragColor;

	//gamma correct
	vec3 corrected_color = pow(color, vec3(0.4545));
	vec3 final_color = clamp(corrected_color, 0.0, 1.0);
	
	outFragColor = vec4(final_color, 1.0);

}