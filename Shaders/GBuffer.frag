#version 450



//layout (location = 0) out vec3 outPosition;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inTangent;
layout (location = 2) in vec3 inBitangent;

layout (location = 3) in vec2 inUV;

//Gbuffer targets vec3/float
layout (location = 0) out vec4 outAlbedoMetallic;
layout (location = 1) out vec4 outNormalRoughness;
layout (location = 2) out vec4 outEmmissiveOcclusion;

//Material
layout(set = 1, binding = 0) uniform sampler2D inAlbedo;
layout(set = 1, binding = 1) uniform sampler2D inNormalTex;
layout(set = 1, binding = 2) uniform sampler2D inOccRoughMetal;
layout(set = 1, binding = 3) uniform sampler2D inEmmissive;

void main()
{

    //Normal mapping
	mat3 mTBN = mat3(normalize(inTangent), normalize(inBitangent), normalize(inNormal));
	vec3 tex_normal = normalize(texture(inNormalTex, inUV).xyz * 2.0 - 1.0); //shift to -1 to 1 range from 0 to 1
	
	outAlbedoMetallic.rgb = texture(inAlbedo, inUV).xyz;
	outAlbedoMetallic.a = texture(inOccRoughMetal, inUV).z;

	outNormalRoughness.rgb = normalize(mTBN * tex_normal);
	outNormalRoughness.a = texture(inOccRoughMetal, inUV).y;
	
	outEmmissiveOcclusion.rgb = texture(inEmmissive, inUV).xyz;
	outEmmissiveOcclusion.a = texture(inOccRoughMetal, inUV).x;

	 
}