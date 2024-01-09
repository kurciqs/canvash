cmake_minimum_required(VERSION 3.26)
project(canvash_lib C)

add_library(${PROJECT_NAME} canvash.c canvash.h shader.c shader.h)

# Resources
configure_file(${PROJECT_SOURCE_DIR}/res/img/icon.png ${CMAKE_BINARY_DIR}/res/img/icon.png COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/res/shader/rect_vert.glsl ${CMAKE_BINARY_DIR}/res/shader/rect_vert.glsl COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/res/shader/rect_frag.glsl ${CMAKE_BINARY_DIR}/res/shader/rect_frag.glsl COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/res/shader/ell_vert.glsl ${CMAKE_BINARY_DIR}/res/shader/ell_vert.glsl COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/res/shader/ell_frag.glsl ${CMAKE_BINARY_DIR}/res/shader/ell_frag.glsl COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/res/shader/tri_vert.glsl ${CMAKE_BINARY_DIR}/res/shader/tri_vert.glsl COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/res/shader/tri_frag.glsl ${CMAKE_BINARY_DIR}/res/shader/tri_frag.glsl COPYONLY)

# Includes
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib/cglm/include)

# Libraries
set(GLFW_BUILD_EXAMPLES false)
set(GLFW_BUILD_TESTS false)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glad)
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/cglm)
target_link_libraries(${PROJECT_NAME} cglm)

# Compiler business for it to always work
IF (WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32)
ENDIF ()