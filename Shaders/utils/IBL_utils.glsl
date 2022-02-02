#ifndef IBL_UTILS_GLSL
#define IBL_UTILS_GLSL

#include "brdf.glsl"

// https://learnopengl.com/PBR/IBL/Specular-IBL
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf


//Converts uv coordinate in equirectmap to a direction in the corresponding cubemap
vec3 cubemapDirection(vec2 uv, uint face)//NOTE: branching in shader probably not good
{
    const vec2 tex_coord = uv * 2.0 - 1.0; //convert to -1 to 1 range

    if(face == 0) return vec3(1.0, -tex_coord.y, -tex_coord.x); // front
    if(face == 1) return vec3(-1.0, -tex_coord.y, tex_coord.x); // back

    if(face == 2) return vec3(tex_coord.x, 1.0, tex_coord.y); // right
    if(face == 3) return vec3(tex_coord.x, -1.0, -tex_coord.y); // left

    if(face == 4) return vec3(tex_coord.x, -tex_coord.y, 1.0); // top
    if(face == 5) return vec3(-tex_coord.x, -tex_coord.y, -1.0); // bottom

}


//Returns UV coordinates of equirect map that corresponds to that cubemap direction
vec2 sampleSphericalMap(vec3 dir)
{
    const vec2 invAtan = vec2(0.1591, 0.3183);

    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv *= invAtan;
    uv += 0.5;

    return uv;
}



float radicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}


vec2 hammersley2d(uint i, uint N)
{
    return vec2(float(i)/float(N), radicalInverse_VdC(i));
}  


// GGX NDF importance sampling
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{

    float alpha2 = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha2*alpha2 - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    //tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = normalize(cross(N, tangent));
	
    vec3 sampleVector = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVector);
}


// Importance sampling for irradaince map
vec3 importanceSampleGGXDiffuse(vec2 Xi, vec3 N)
{


    float phi = 2.0 * PI * Xi.x;
    float cosTheta = (1.0 - Xi.y) ;
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    //tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = normalize(cross(N, tangent));
	
    vec3 sampleVector = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVector);
}



//BRDF integration
vec2 integrateBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; i++)
    {
        vec2 Xi = hammersley2d(i, SAMPLE_COUNT);
        vec3 H  = importanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = G_Smith(NdotV, NdotL, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

#endif