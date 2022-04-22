#version 450



//layout (location = 0) out vec3 outPosition;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inTangent;
layout (location = 2) in vec3 inBitangent;

layout (location = 3) in vec2 inUV;

//Gbuffer targets vec3/float
layout (location = 0) out vec4 outAlbedo;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outOccRoughMetal;
layout (location = 3) out vec4 outEmmissive;

//Material
layout(set = 1, binding = 0) uniform sampler2D inAlbedo;
layout(set = 1, binding = 1) uniform sampler2D inNormalTex;
layout(set = 1, binding = 2) uniform sampler2D inOccRoughMetal;
layout(set = 1, binding = 3) uniform sampler2D inEmmissive;

void main()
{

    //Normal mapping
	mat3 mTBN = mat3(normalize(inTangent), normalize(inBitangent), normalize(inNormal));
	vec3 tex_normal = texture(inNormalTex, inUV).xyz; //shift to -1 to 1 range from 0 to 1
	vec3 normal_ts = normalize( tex_normal* 2.0 - vec3(1.0)); //shift to -1 to 1 range from 0 to 1
	
	vec3 normal = normalize(mTBN * normal_ts);
	vec3 packed_normal = (normal * 0.5) + vec3(0.5);

	outAlbedo = vec4(texture(inAlbedo, inUV).xyz, 1.0);
	outNormal = vec4(packed_normal, 1.0);
	outOccRoughMetal = vec4(texture(inOccRoughMetal, inUV).xyz, 1.0);
	outEmmissive = vec4(texture(inEmmissive, inUV).xyz, 1.0);

	 
}