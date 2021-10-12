#version 450 

#include "utils/brdf.glsl"

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
layout(set = 1, binding = 3) uniform sampler2D inEmmissive;





//PBR Lo (outgoing radiance) calculation

vec3 calcDiffuseSpecPBR(vec3 normal, vec3 light_dir, vec3 view_dir, vec3 albedo, float roughness, float metallic){

	vec3 H = normalize(light_dir + view_dir);
	float NdotV = max(dot(normal, view_dir), 0.0);
	float NdotL = max(dot(normal, light_dir), 0.0);
	float NdotH = max(dot(normal, H), 0.0);
	float HdotV = max(dot(H, view_dir), 0.0);

	vec3 F0 = approximateFO(albedo, metallic);

	vec3 F = F_Schlick(HdotV, F0);
	float G = G_Smith(NdotV, NdotL, roughness);
	float NDF = D_GGX(NdotH, roughness);

	vec3 Ks = F;
	vec3 Kd = (1.0 - Ks) * (1.0 - metallic);
	
	vec3 specular = Ks * max(NDF*G/getCookTorranceDenom(NdotL,NdotV), 0.0);
	vec3 diffuse = Kd * albedo / PI;

	return NdotL * (diffuse + specular);

}



float calcAttenuation(float radius, vec3 light_pos, vec3 fragPos){
	float dist = length(light_pos-fragPos);
	float k_linear = 2.0/radius;
	float k_quadratic = 1.0/(radius*radius);
	return 1.0/(1.0 + k_linear*dist + k_quadratic*dist*dist); //TODO implement cutoff	
}






void main()
{
	//Normal mapping
	mat3 mTBN = mat3(normalize(inTangent), normalize(inBitangent), normalize(inNormal));
	vec3 tex_normal = normalize(texture(inNormalTex, inUV).xyz * 2.0 - 1.0);
	
	//
	vec3 N = normalize(mTBN * tex_normal);
	vec3 V = -normalize(inFragPos);
	vec3 albedo = texture(inAlbedo, inUV).xyz; 
	float roughness = texture(inOccRoughMetal, inUV).y;
	float metallic = texture(inOccRoughMetal, inUV).z;
	float occlusion = texture(inOccRoughMetal, inUV).x;
	vec3 emmissive = texture(inEmmissive, inUV).xyz;


	//Directional lights
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < scene_info.dir_light_count; i++)
	{
		vec3 L = dir_lights[i].direction;
		vec3 radiance = dir_lights[i].colour * dir_lights[i].intensity;
		Lo += radiance * calcDiffuseSpecPBR(N, L, V, albedo, roughness, metallic);

	}

	//Point lights
	for (int i = 0; i < scene_info.point_light_count; i++)
	{
		vec3 L = normalize(point_lights[i].position - inFragPos);
		float attenuation = calcAttenuation(point_lights[i].radius, point_lights[i].position, inFragPos);
		
		vec3 radiance = point_lights[i].colour * attenuation * point_lights[i].intensity;
		Lo += radiance * calcDiffuseSpecPBR(N, L, V, albedo, roughness, metallic);

	}

	vec3 ambient = vec3(0.01) * albedo * occlusion;
    vec3 color = Lo + ambient + emmissive;
    color = color / (color + vec3(1.0));
	vec3 gamma_corrected_color = pow(color, vec3(0.4545));
	vec3 final_color = clamp(gamma_corrected_color, 0.0, 1.0);
	outFragColor = vec4(final_color, 1.0);

}