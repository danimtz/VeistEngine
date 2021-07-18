cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project ("VulkanEngine")

find_package(Vulkan REQUIRED)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


add_subdirectory(vendor)

add_subdirectory(src)



## from vkguide.dev

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin32/ $ENV{VULKAN_SDK}/Bin/)

## find all the shader files under the shaders folder
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/Shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/src/Shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/src/Shaders/*.comp"
    )

message(STATUS ${GLSL_VALIDATOR})
## iterate each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/src/Shaders/${FILE_NAME}.spv")
  message(STATUS ${GLSL})

  ##execute glslang command to compile that specific shader
  #add_custom_command(
  #  OUTPUT ${SPIRV}
  #  COMMENT "compile shaders"
  #  COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV} 
  #  DEPENDS ${GLSL}
  #)
  execute_process(
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV} 
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

#add_custom_target(
#    Shaders 
#    DEPENDS ${SPIRV_BINARY_FILES}
#    )

