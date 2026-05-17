cmake_minimum_required(VERSION 3.10)
project(DVDBounce)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.9
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Executable
add_executable(DVDBounce
  src/main.cpp
  src/glad.c
)

target_include_directories(DVDBounce PRIVATE
  src/include/
)

target_link_libraries(DVDBounce PRIVATE glfw)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets  DESTINATION ${CMAKE_BINARY_DIR})