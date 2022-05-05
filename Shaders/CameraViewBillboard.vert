#version 460

#include "utils/utils.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 padding1;
layout (location = 2) in vec4 padding2;
layout (location = 3) in vec2 padding3;

//layout (location = 0) out vec3 outPosition;
layout (location = 0) out vec2 outUV;

layout (push_constant) uniform constants
{
	vec4 uv1uv2;
}pConstant;


layout(set = 0, binding = 0) uniform  cameraBuffer
{
	mat4 mV;
	mat4 mP;
	mat4 mVP;
	mat4 mInvV;
} camera;

layout(std140, set = 0, binding = 1) readonly buffer objectMatrices
{
	ObjectMatrices object_matrices[];
};



vec2 getUVcoordinates(vec4 uv1uv2, uint vertex)//branching in shader probably not good
{

    if(vertex == 2) return vec2(uv1uv2.x, uv1uv2.y); // top left
    if(vertex == 3) return vec2(uv1uv2.z, uv1uv2.y); // top right

    if(vertex == 0) return vec2(uv1uv2.x, uv1uv2.w); // bottom left
    if(vertex == 1) return vec2(uv1uv2.z, uv1uv2.w); // bottom right

}


void main()
{

	const float billboard_size = 0.5;

	vec3 center_position_ws = vec3(object_matrices[gl_BaseInstance].mModel[3][0], object_matrices[gl_BaseInstance].mModel[3][1], object_matrices[gl_BaseInstance].mModel[3][2]);
	vec3 camera_right_ws = vec3(camera.mInvV[0][0], camera.mInvV[0][1], camera.mInvV[0][2]);
	vec3 camera_up_ws = vec3(camera.mInvV[1][0], camera.mInvV[1][1], camera.mInvV[1][2]);

	vec3 vertex_position_ws = center_position_ws +
								camera_right_ws * inPosition.x * billboard_size +
								 camera_up_ws * inPosition.y * billboard_size;

	//output the position of each vertex
	gl_Position = ((camera.mVP)*vec4(vertex_position_ws, 1.0f));
	

	//Calculate UV using push constant ranges
	

	
	outUV = getUVcoordinates( pConstant.uv1uv2, gl_VertexIndex);//TODO change


}