
file(GLOB_RECURSE SRC_FILES "*.cpp" "*.h")

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} vma glm stb_image tinygltf nlohmann )
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} spirv-cross-core)
target_link_libraries(${PROJECT_NAME} spirv-cross-glsl)
target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)


#Theres gotta be a better way to do this
target_link_libraries(${PROJECT_NAME} optimized "$ENV{VULKAN_SDK}/Lib/shaderc.lib")
target_link_libraries(${PROJECT_NAME} optimized "$ENV{VULKAN_SDK}/Lib/shaderc_combined.lib")
target_link_libraries(${PROJECT_NAME} optimized "$ENV{VULKAN_SDK}/Lib/shaderc_shared.lib")
target_link_libraries(${PROJECT_NAME} optimized "$ENV{VULKAN_SDK}/Lib/shaderc_util.lib")

target_link_libraries(${PROJECT_NAME} debug "$ENV{VULKAN_SDK}/Lib/shaderc_combinedd.lib")
target_link_libraries(${PROJECT_NAME} debug "$ENV{VULKAN_SDK}/Lib/shaderc_sharedd.lib")
target_link_libraries(${PROJECT_NAME} debug "$ENV{VULKAN_SDK}/Lib/shaderc_utild.lib")
target_link_libraries(${PROJECT_NAME} debug "$ENV{VULKAN_SDK}/Lib/shadercd.lib")

message(STATUS ${Vulkan_INCLUDE_DIRS})


target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)


#add_dependencies(${PROJECT_NAME} Shaders)
