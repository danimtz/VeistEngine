#version 460 

#include "utils/utils.glsl"

#define SSAO_KERNEL_SIZE 64



layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  cameraBuffer
{
	Camera camera_data;
};

layout (set = 0, binding = 1) uniform sampler2D inGNormal;
layout (set = 0, binding = 2) uniform sampler2D inGDepth;
layout (set = 0, binding = 3) uniform sampler2D inNoiseTexture;

layout( set = 0, binding = 4) uniform samples{
    vec3 SSAOsamples[SSAO_KERNEL_SIZE];
};

layout( push_constant ) uniform p_constant{
    vec2 noise_scale;
};

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy);
	float depth = texelFetch(inGDepth, coord, 0).x;
    
    vec3 frag_pos = positionFromDepth(inUV, depth, camera_data.mInvP);
    vec3 texture_normal = texture(inGNormal, inUV).xyz;

    vec3 random_vec = texture(inNoiseTexture, inUV * noise_scale).xyz;  

    vec3 tangent = normalize( random_vec - texture_normal * dot(random_vec, texture_normal) ); //Gram schmidt nothogonalizastion
    vec3 bitangent = cross(texture_normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, texture_normal);


    
    float occlusion = 0.0f;
    float hemisphere_radius = 0.5f;
    float bias = 0.025f;

    //Sample hemisphere
    for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
    {
        
        vec3 sample_pos = TBN * SSAOsamples[i]; //view space sample pos
        sample_pos = frag_pos + sample_pos * hemisphere_radius;

      
        vec4 offset = vec4(sample_pos, 1.0f);
        offset = camera_data.mP * offset; //to NDC
        offset.xyz /= offset.w; //perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; //to texture space (0.0 to 1.0)

        float sample_depth = positionFromDepth(inUV, depth, camera_data.mInvP).z;

        float range_check = smoothstep(0.0, 1.0, hemisphere_radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;  
        
    }

    occlusion = 1.0 - (occlusion / SSAO_KERNEL_SIZE);
    outFragColor.x = occlusion;  
}