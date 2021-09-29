# VulkanEngine
Vulkan rendering engine written with the objective of learning how the basics of the graphics API.  
Currently a work in progress, but the objective is to be able to set up forward rendering, deferred rendering and maybe forward+/clustered rendering.

## Current features:
### Engine:
- Spir-V reflection of glsl shader to create descriptor set layouts.
- Free camera movement using quaternions.
- GLTF2.0 model loading
- Uses GLFW for window mangement
- ImGUI integration

### Renderer:
- Forward renderer
- Double buffering
- Physically Based Shading (Cook Torrance BRDF, no IBL for now)
- Cubemapped skybox

## Example images:


## Resources:
 Main resources being used are:
 - [Vulkan tutorial](https://vulkan-tutorial.com/)   
 - [VKguide by vblanco20-1](https://vkguide.dev/)

## External libraries used:
- [stbimage](https://github.com/nothings/stb) for texture/image loading.
- [dearImgui](https://github.com/ocornut/imgui) for GUI.
- [GLFW](https://www.glfw.org/) for hardware window abstraction.
- [glm](https://github.com/g-truc/glm) math library originally written for openGL.
- [vma](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) Vulkan Memory Allocator.
- [Tinygltf](https://github.com/syoyo/tinygltf) Header only file for loading gltf parser.
- [Spirv-Cross](https://github.com/KhronosGroup/SPIRV-Cross) Used for automatic shader reflection of descriptor layouts.



## Models and Textures:
- Water bottle
- Damaged Helmet
- Reyjfallet skybox
