#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL




float calcShadows(sampler2DShadow shadow_map, mat4 mLightSpace, vec3 world_pos, float NdotL)
{
	float bias = max(0.01 * (1.0 - NdotL), 0.005);

	vec3 texel = project(world_pos, mLightSpace);

	


	float shadow_test = texture(shadow_map, vec3(texel.xy, texel.z - bias));
	
	//float ret = (shadow_test == 1.0) ? 1.0 : 0.0;

	return shadow_test;
}




#endif