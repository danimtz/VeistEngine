#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL



float calcShadows(sampler2DShadow shadow_map, mat4 mLightSpace, vec3 world_pos, float NdotL)
{
	float bias = max(0.05 * (1.0 - NdotL), 0.005);

	vec4 shadow_coord = (mLightSpace * vec4(world_pos, 1.0)); 
	shadow_coord /= shadow_coord.w;

	shadow_coord.x = shadow_coord.x * 0.5 + 0.5; //convert to 0 to 1 range
	shadow_coord.y = shadow_coord.y * 0.5 + 0.5;
	shadow_coord.z -= bias;


	float shadow_test = texture(shadow_map, shadow_coord.xyz);
	
	float ret = (shadow_test == 1.0) ? 1.0 : 0.0;

	return ret;
}




#endif