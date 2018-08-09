cmake_minimum_required(VERSION 3.5)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

project(chip8 C)

add_executable(chip8 main.c chip8.c)
target_link_libraries(chip8 glfw ${GLFW_STATIC_LIBRARIES} ${OPENGL_LIBRARIES} openal)
