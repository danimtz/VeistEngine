# VeistEngine
Veist is a vulkan rendering engine written with the objective of learning how the basics of the graphics API and game engines.  
Currently a work in progress, it has several features already such as a working proof of concept editor, a rendergraph system to easily setup the rendering
architecture and automatically handle and manage device resources. Simple deferred and forward rendering have been implemented, with clustered/forward+ to be implemented soon on the renderer side. The engine and editor themselves are also a work in progress.

## Current features:
### Engine:
- Spir-V reflection of glsl shader to create descriptor set layouts.
- Command buffer recording abstraction.
- Free camera movement using quaternions.
- GLTF2.0 model loading
- Uses GLFW for window mangement
- ImGUI based dditor built as a separate program
- Entity component system
- Movable transform components
- RenderGraph rendering system (Allows renderer architecture to be swapped mid program from deferred to forward)

### Renderer:
- Deferred fenderer
- Forward renderer
- Double buffering
- Directional and Point lights
- Physically Based Rendering (Cook Torrance BRDF)
- Image-Based Lighting (calculates diffuse irradince maps, BRDF LUTs and pre-filtered environment map using compute shaders)
- Cubemapped skybox
- ShadowMapping


## Example images:
**Editor and Sponza scene loaded. Shadow mapping and render target options**  
<img src="https://raw.githubusercontent.com/danimtz/VulkanEngine/master/assets/RenderImages/Shadowmapping.PNG" width="950" height=100%> 

**Editor and Sponza scene loaded. Deferred PBR renderer with point and directional lights. 2D billboard icons for lights (no shadows yet)**     
<img src="https://raw.githubusercontent.com/danimtz/VulkanEngine/master/assets/RenderImages/Editor.PNG" width="950" height=100%>     

**Image Based Lighting. Only ambient light is being used in this example (old)**     
<img src="https://raw.githubusercontent.com/danimtz/VulkanEngine/master/assets/RenderImages/IBL.png" width="950" height=100%>     

**Simple PBR shading with directional light(sun) and two point lights (older)**     
<img src="https://raw.githubusercontent.com/danimtz/VulkanEngine/master/assets/RenderImages/SkyBox_PBRtest2.PNG" alt="Skybox sample" width="950" height=100%>     


## Download:
Although the project is fully for educational purposes feel free to download it if you so want. I cant guarantee it will fully function. Once cloned, cmake can be used to build the project however a few errors might show up:
- Make sure to "git submodule init" and "git submodule update" to initialize a few of the libraries.
- In Cmake untick SPIRV_CROSS_ENABLE_TEST if it is enabled.
- If the project gives errors when running about missing shader files, the working directory of the Editor project might be one directory out. It should be in VeistEngine/bin/XXXXX(Debug, Release etc...).


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
- [Water Bottle](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/WaterBottle) from Khronos group sample GTLF2.0. models.
- [Damaged Helmet](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/DamagedHelmet) from Khronos group sample GTLF2.0. models.
- [Flight Helmet](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/FlightHelmet) from Khronos group sample GTLF2.0. models.
- [Sponza](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Sponza) from Khronos group sample GTLF2.0. models.
- [Ryfjället skybox](https://www.humus.name/index.php?page=Textures&ID=37) from HUMUS textures.
- [Venice HDR sunset](https://polyhaven.com/a/venice_sunset) from polyhaven.
- [Ice lake HDR skybox](http://www.hdrlabs.com/sibl/archive.html) from sIBL archive.
