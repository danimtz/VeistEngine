

#Build editor
file(GLOB_RECURSE EDITOR_SRC_FILES "*.cpp" "*.h")

add_executable(Editor ${EDITOR_SRC_FILES})

target_link_libraries(Editor VeistEngine)

target_include_directories(Editor PUBLIC "${PROJECT_SOURCE_DIR}/Engine" "${PROJECT_SOURCE_DIR}/Editor")

set(editor_src_root_path "${PROJECT_SOURCE_DIR}/Editor")
foreach(editor_source IN ITEMS ${ENGINE_SRC_FILES})
    get_filename_component(editor_source_path "${editor_source}" PATH)
    file(RELATIVE_PATH editor_source_path_rel "${editor_src_root_path}" "${editor_source_path}")
    string(REPLACE "/" "\\" editor_group_path "${editor_source_path_rel}")
    source_group("${editor_group_path}" FILES "${editor_source}")
endforeach()

target_precompile_headers(Editor PRIVATE "${PROJECT_SOURCE_DIR}/Engine/pch.h")


set_property(TARGET Editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")