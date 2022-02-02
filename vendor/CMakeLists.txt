find_package(Vulkan REQUIRED)

add_library(glm INTERFACE)
add_library(vma INTERFACE)
add_library(stb_image INTERFACE)
add_library(tinygltf INTERFACE)
add_library(nlohmann INTERFACE)


# vma, glm and stbimage are header only, include path needed only
target_include_directories(vma INTERFACE vma)
target_include_directories(glm INTERFACE glm)
target_include_directories(stb_image INTERFACE stb_image)
target_include_directories(tinygltf INTERFACE tinygltf)
target_include_directories(nlohmann INTERFACE nlohmann)


#Add shaderc includer support
add_library(shaderc_file_includer STATIC)
target_include_directories(shaderc_file_includer PUBLIC shaderc_file_includer)
target_include_directories(shaderc_file_includer PUBLIC "$ENV{VULKAN_SDK}/Include" )
target_sources(shaderc_file_includer PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/shaderc_file_includer/file_includer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaderc_file_includer/file_includer.cc"
    )

target_link_libraries(shaderc_file_includer optimized "$ENV{VULKAN_SDK}/Lib/shaderc.lib")
target_link_libraries(shaderc_file_includer optimized "$ENV{VULKAN_SDK}/Lib/shaderc_combined.lib")
target_link_libraries(shaderc_file_includer optimized "$ENV{VULKAN_SDK}/Lib/shaderc_shared.lib")
target_link_libraries(shaderc_file_includer optimized "$ENV{VULKAN_SDK}/Lib/shaderc_util.lib")


target_link_libraries(shaderc_file_includer debug "$ENV{VULKAN_SDK}/Lib/shaderc_combinedd.lib")
target_link_libraries(shaderc_file_includer debug "$ENV{VULKAN_SDK}/Lib/shaderc_sharedd.lib")
target_link_libraries(shaderc_file_includer debug "$ENV{VULKAN_SDK}/Lib/shaderc_utild.lib")
target_link_libraries(shaderc_file_includer debug "$ENV{VULKAN_SDK}/Lib/shadercd.lib")




# Add glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.3.4)

# Add spirv cross
add_subdirectory(spirv-cross)


#TODO LINK SPIRV CROSS ETC ETC ETC


# target_link_libraries(myapp glfw)

# Add imgui library
add_library(imgui STATIC)

target_include_directories(imgui PUBLIC imgui imgui/backends)

target_sources(imgui PRIVATE 
   
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    )

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)
