#ifndef BRDF_GLSL
#define BRDF_GLSL

#define PI 3.1415926538
//LearnOpenGL
//Google filament

//BRDF functions

vec3 F_Schlick(float HdotV, vec3 F0){
	float neg_HdotV = max(1.0-HdotV, 0.0);
	return F0 + (1.0 - F0) * (neg_HdotV*neg_HdotV*neg_HdotV*neg_HdotV*neg_HdotV);//(1-cos_theta)^5 but more efficient
}

vec3 F_SchlickRoughness(float HdotV, vec3 F0, float roughness){
	float neg_HdotV = max(1.0-HdotV, 0.0);
	return F0 + ( max(vec3(1.0 - roughness), F0) - F0) * (neg_HdotV*neg_HdotV*neg_HdotV*neg_HdotV*neg_HdotV);//(1-cos_theta)^5 but more efficient
}


float D_GGX(float NdotH, float roughness){
	float a = roughness*roughness;
	float a2 = a*a;
	float denom = (NdotH * (a2 - 1.0) + 1.0);
	return a2/(PI*denom*denom);
}

float G_SchlickGGX(float NdotV, float roughness){
	float r = roughness+1.0;
	float k = (r*r)/8.0;
	float denom = NdotV * (1.0 - k) + k;
	return NdotV / denom;
}

float G_Smith(float NdotV, float NdotL, float roughness){
	return G_SchlickGGX(NdotL, roughness) * G_SchlickGGX(NdotV, roughness);
}

float getCookTorranceDenom(float NdotL, float NdotV){
	return max(4.0*NdotV*NdotL, 0.001);
}



vec3 approximateFO(vec3 albedo, float metallic){
	return mix(vec3(0.04), albedo, metallic);
}


#endif