#version 460

#include "utils/utils.glsl"
#include "utils/brdf.glsl"
#include "utils/shadows.glsl"

#define MAX_DIR_LIGHTS 4


layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  sceneInfo{
	float dir_light_count;
	float point_light_count;
}scene_info;


layout(set = 0, binding = 1) uniform  cameraBuffer
{
	Camera camera_data;
};


layout(set = 0, binding = 2) uniform  directionalLights
{
	DirLights dir_lights[MAX_DIR_LIGHTS];
};


layout(std140, set = 0, binding = 3) readonly buffer pointLights
{
	PointLights point_lights[];
};





layout(set = 0, binding = 4) uniform samplerCube inIrradianceMap;
layout(set = 0, binding = 5) uniform samplerCube inPrefilterMap;
layout(set = 0, binding = 6) uniform sampler2D inBRDF_LUT;

layout (set = 0, binding = 7) uniform sampler2D inAlbedo;
layout (set = 0, binding = 8) uniform sampler2D inNormal;
layout (set = 0, binding = 9) uniform sampler2D inOccRoughMetal;
//layout (set = 0, binding = 10) uniform sampler2D inEmmissive;
layout (set = 0, binding = 10) uniform sampler2D inDepth;

layout(set = 0, binding = 11) uniform sampler2DShadow inShadows;
layout(set = 0, binding = 12) uniform shadowsDataBuffer
{
	ShadowMapData shadow_data[MAX_DIR_LIGHTS];
};



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

	mat4 mInvV_no_trans = mat4(mat3(camera_data.mInvV));

	ivec2 coord = ivec2(gl_FragCoord.xy);
	float depth = texelFetch(inDepth, coord, 0).x;
	vec3 frag_pos = positionFromDepth(inUV, depth, camera_data.mInvP);

	vec3 world_space_pos = vec3(camera_data.mInvV * vec4(frag_pos, 1.0));

	vec3 texture_normal = texture(inNormal, inUV).xyz;
	vec3 N = normalize( texture_normal* 2.0 - vec3(1.0)); 
	vec3 worldN = vec3(mInvV_no_trans * vec4(N, 1.0)); 

	vec3 V = -normalize(frag_pos);
	vec3 worldR = normalize(vec3( mInvV_no_trans * vec4(reflect(-V, N), 1.0))); 

	vec3 albedo = texture(inAlbedo, inUV).xyz; 
	float roughness = texture(inOccRoughMetal, inUV).y;
	float metallic = texture(inOccRoughMetal, inUV).z;
	float occlusion = texture(inOccRoughMetal, inUV).x;
	//vec3 emmissive = texture(inEmmissive, inUV).xyz;


	//Directional lights
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < scene_info.dir_light_count; i++)
	{
		vec3 L = dir_lights[i].direction;
		vec3 radiance = dir_lights[i].colour * dir_lights[i].intensity;


		float attenuation = calcShadows(inShadows, shadow_data[i].mLightSpaceMatrix, world_space_pos, dot(N,L));
		
		//float TESTSHADOW = texture(inShadows, vec3(inUV, 1.0)).x;

		radiance *= attenuation;
		Lo += radiance * calcDiffuseSpecPBR(N, L, V, albedo, roughness, metallic);


	}

	//Point lights
	for (int i = 0; i < scene_info.point_light_count; i++)
	{
		vec3 L = normalize(point_lights[i].position - frag_pos);
		float attenuation = calcAttenuation(point_lights[i].radius, point_lights[i].position, frag_pos);
		
		vec3 radiance = point_lights[i].colour * attenuation * point_lights[i].intensity;
		Lo += radiance * calcDiffuseSpecPBR(N, L, V, albedo, roughness, metallic);

	}

	//IBL
	vec3 F0 = approximateFO(albedo, metallic);
	vec3 F = F_SchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS =  F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;	
	vec3 irradiance = texture(inIrradianceMap, worldN).rgb;
	vec3 ambient_diffuse = irradiance * albedo; //Diffuse

	const uint MAX_REFLECTION_LOD = textureQueryLevels(inPrefilterMap);
    vec3 prefilteredColor = textureLod(inPrefilterMap, worldR,  roughness * (MAX_REFLECTION_LOD - 1.0)).rgb;
	vec2 envBRDF  = texture(inBRDF_LUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 ambient_specular = prefilteredColor * (F * envBRDF.x + envBRDF.y); //Specular


	vec3 ambient = (kD * ambient_diffuse + ambient_specular );// * occlusion; //Sponza model textures dont have occlusion texture set to 1.0 
	
	
	if(depth == 1.0) //Small patch not really needed, this should really be handled with light volumes etc, 
	{//If theres no fragment on depth buffer, the entire shader shouldnt be running anyway.
		ambient = vec3(0.0,0.0,0.0);
		//emmissive = vec3(0.0,0.0,0.0);
	}

	vec3 color = Lo + ambient ;//+ emmissive;
	//color = color / (color + vec3(1.0)); //reindhardt tone map
	color = pow(color, vec3(0.4545)); //GAMMA CORRECTION
	vec3 final_color = clamp(color, 0.0, 1.0);

	outFragColor = vec4(final_color, 1.0);
}