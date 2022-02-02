#version 450

#define MAX_DIR_LIGHTS 4


struct DirLights{
	vec3 direction;
	float intensity;
	vec3 colour;
	uint padding;//TODO REPLACE WITH PADDING.THIS IS DEPRECATED
};

struct PointLights{
	vec3 position;
	float intensity;
	vec3 colour;
	float radius;
};

layout (location = 0) in  vec3 inFragPos;

layout (location = 1) in  vec3 inNormal;
layout (location = 2) in  vec3 inTangent;
layout (location = 3) in  vec3 inBitangent;

layout (location = 4) in  vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  sceneInfo{
	float dir_light_count;
	float point_light_count;
}scene_info;

layout(set = 0, binding = 2) uniform  directionalLights{
	DirLights dir_lights[MAX_DIR_LIGHTS];
};

layout(std140, set = 0, binding = 3) readonly buffer pointLights{
	PointLights point_lights[];
};

layout(set = 1, binding = 0) uniform sampler2D inAlbedo;
layout(set = 1, binding = 1) uniform sampler2D inNormalTex;
layout(set = 1, binding = 2) uniform sampler2D inOccRoughMetal;



void main()
{
	//Normal mapping
	mat3 mTBN = mat3(normalize(inTangent), normalize(inBitangent), normalize(inNormal));
	vec3 tex_normal = normalize(texture(inNormalTex, inUV).xyz * 2.0 - 1.0);
	vec3 normal = normalize(mTBN * tex_normal);
	
	vec3 occ = texture(inOccRoughMetal, inUV).xyz;


	vec3 total_light = vec3(0.0);

	float kd = 0.8;//should be from material
	float ks = 0.3;

	for(int i = 0; i < scene_info.dir_light_count; i++){

		vec3 light_dir = dir_lights[i].direction;
		vec3 light_colour = dir_lights[i].colour;
		float intensity = dir_lights[i].intensity;
		float diffuse = max(0.0, dot(normal, light_dir));

		//specular TODO
		vec3 h = normalize(light_dir + -normalize(inFragPos));
 		float spec = pow(max(0.0, dot(normal, h)), 128.0);//128 from material too 
		spec = diffuse > 0.0 ? spec : 0.0;

		total_light +=  light_colour * (diffuse*kd + spec*ks)  * intensity;

	}
	
	//point lights
	for (int i = 0; i < scene_info.point_light_count; i++)
	{
		vec3 light_dir = normalize(point_lights[i].position - inFragPos);
		vec3 light_colour = point_lights[i].colour;
		float intensity = point_lights[i].intensity;
		float diffuse = max(0.0, dot(normal, light_dir));

		float dist = length(point_lights[i].position-inFragPos);
		float k_linear = 2.0/point_lights[i].radius;
		float k_quadratic = 1.0/(point_lights[i].radius*point_lights[i].radius);
		float attenuation = 1.0/(1.0 + k_linear*dist + k_quadratic*dist*dist); //TODO implement cutoff
	
		

		vec3 h = normalize(light_dir + -normalize(inFragPos));
 		float spec = pow(max(0.0, dot(normal, h)), 128.0);//128 from material too 

		total_light +=  light_colour * (diffuse*kd + spec*ks)  * intensity * attenuation ;
	}

	
	//temporary  
	vec3 texture_color = texture(inAlbedo, inUV).xyz; //change to sample those coordinates

	vec3 color =  total_light * texture_color;

	
	color += texture_color*0.005; //ambient light
	
	//gamma correct
	vec3 corrected_color = pow(color, vec3(0.4545));
	vec3 final_color = clamp(corrected_color, 0.0, 1.0);
	

	//outFragColor = vec4((normal), 1.0);//
	outFragColor = vec4(final_color, 1.0);

}