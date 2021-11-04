#ifndef IBL_UTILS_GLSL
#define IBL_UTILS_GLSL

const float PI = 3.14159265358979323846f;

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


#endif