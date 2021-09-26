#version 450


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 padding1;
layout (location = 2) in vec4 padding2;
layout (location = 3) in vec2 padding3;

layout (location = 0) out vec3 outPosition;

//layout (location = 1) out vec3 outNormal;
//layout (location = 2) out vec3 outTangent;
//layout (location = 3) out vec3 outBitangent;
//layout (location = 4) out vec2 outUV;



layout( push_constant ) uniform constants
{
	mat4 mVP;
	mat4 padding;
} push_constant;



void main()
{
	
	//output the position of each vertex
	gl_Position = (push_constant.mVP)*vec4(inPosition, 1.0f);
	outPosition = inPosition;
	
	//mat3 Nmat = mat3(push_constant.mN); //mN is already a 3x3 extended to a 4x4 matrix but this is cleaner
	//outNormal = normalize(Nmat * inNormal);
	//outTangent = normalize(Nmat * inTangent.xyz);
	//outBitangent = normalize(cross(outNormal, outTangent) * inTangent.w);

	//outUV = inUV;

}