#ifndef SH_UTILS_GLSL
#define SH_UTILS_GLSL

//#include "constants.glsl" 
//http://orlandoaguilar.github.io/sh/spherical/harmonics/irradiance/map/2017/02/12/SphericalHarmonics.html



struct SHB
{
    float Y00, Y11, Y10, Y1_1, Y22, Y21, Y20, Y2_1, Y2_2 
}

/*
struct SH
{
    vec4 L00, L11, L10, L1_1, L22, L21, L20, L2_1, L2_2 
}
*/

SH_Basis getBasis(vec3 dir)
{
    float x = dir.x;
    float y = dir.y;
    flaot z = dir.z;
	return SH_Basis(3
		1,
		y,
		z,
		x,
		x*y,
		y*z,
		(3 * z*z - 1.0f),
		x*z,
		(x*x - y*y)
	);

}

SH_Basis multCoefficients(SH_Basis basis){

    return SH_Basis(
		0.282095f * 3.141593f * basis.Y00,		
		-0.488603f * 2.094395f * basis.Y11,
		0.488603f * 2.094395f * basis.Y10,
		-0.488603f * 2.094395f * basis.Y1_1,
		1.092548f * 0.785398f * basis.Y22,
		-1.092548f * 0.785398f * basis.Y21,
		0.315392f * 0.785398f * basis.Y20,
		-1.092548f * 0.785398f * basis.Y2_1,
		0.546274f * 0.785398f * basis.Y2_2
		);
}


#endif


//remove HLSL from nvidia exmaple
float2 map_to_tile_and_offset( tile, offsetTexCoord )  
{    return (tile / numTiles) + (offsetTexCoord / (numTiles * numSamples))  }    

float3 SH_Projection( inputEnvironmentMap, dωYlm, currentLlm : VPOS ) : COLOR    
{    
	sum = 0    
	for_all {T0 : inputEnvironmentMap}      
		T1 = map_to_tile_and_offset(currentLlm, T0)     
		weighted_SHbasis = dωYlm[T1]     
		radiance_Sample = inputEnvironmentMap[T0]   
		sum += radiance_Sample * weighted_SHbasis  
	
	return sum  
}
