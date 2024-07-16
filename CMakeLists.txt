cmake_minimum_required(VERSION 3.26)
project(canvash_dev C)

set(CMAKE_C_STANDARD 23)

# Flags and Optimization
IF(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -O3")
ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
ENDIF()

add_subdirectory(canvash_lib)

# Executable
set(SOURCE_FILES main.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} canvash_lib)

# Compiler business for it to always work
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # GCC / MinGW
    target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif()