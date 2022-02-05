
#Build Engine
file(GLOB_RECURSE ENGINE_SRC_FILES "*.cpp" "*.h")

if(VEIST_BUILD_SHARED)
        add_library(VeistEngine SHARED ${ENGINE_SRC_FILES})
    else()
        add_library(VeistEngine STATIC ${ENGINE_SRC_FILES})
    endif()


#Neatly display source folders in visual studio instead of all in header files and source files
set(engine_src_root_path "${PROJECT_SOURCE_DIR}/Engine")
foreach(engine_source IN ITEMS ${ENGINE_SRC_FILES})
    get_filename_component(engine_source_path "${engine_source}" PATH)
    file(RELATIVE_PATH engine_source_path_rel "${engine_src_root_path}" "${engine_source_path}")
    string(REPLACE "/" "\\" engine_group_path "${engine_source_path_rel}")
    source_group("${engine_group_path}" FILES "${engine_source}")
endforeach()


#include_directories("${PROJECT_SOURCE_DIR}/Engine")
target_include_directories(VeistEngine PUBLIC "${PROJECT_SOURCE_DIR}/Engine")

target_link_libraries(VeistEngine vma glm stb_image tinygltf nlohmann)
target_link_libraries(VeistEngine glfw)
target_link_libraries(VeistEngine spirv-cross-core)
target_link_libraries(VeistEngine spirv-cross-glsl)
target_link_libraries(VeistEngine imgui)
target_link_libraries(VeistEngine shaderc_file_includer)
target_link_libraries(VeistEngine Vulkan::Vulkan)

#Theres gotta be a better way to do this
target_link_libraries(VeistEngine optimized "$ENV{VULKAN_SDK}/Lib/shaderc.lib")
target_link_libraries(VeistEngine optimized "$ENV{VULKAN_SDK}/Lib/shaderc_combined.lib")
target_link_libraries(VeistEngine optimized "$ENV{VULKAN_SDK}/Lib/shaderc_shared.lib")
target_link_libraries(VeistEngine optimized "$ENV{VULKAN_SDK}/Lib/shaderc_util.lib")


target_link_libraries(VeistEngine debug "$ENV{VULKAN_SDK}/Lib/shaderc_combinedd.lib")
target_link_libraries(VeistEngine debug "$ENV{VULKAN_SDK}/Lib/shaderc_sharedd.lib")
target_link_libraries(VeistEngine debug "$ENV{VULKAN_SDK}/Lib/shaderc_utild.lib")
target_link_libraries(VeistEngine debug "$ENV{VULKAN_SDK}/Lib/shadercd.lib")



message(STATUS ${Vulkan_INCLUDE_DIRS})
message(STATUS "${PROJECT_SOURCE_DIR}/Engine")

target_precompile_headers(VeistEngine PRIVATE "${PROJECT_SOURCE_DIR}/Engine/pch.h")


set_property(TARGET VeistEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
