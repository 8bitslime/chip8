cmake_minimum_required(VERSION 3.5)
project(chip8)

add_executable(chip8 main.c chip8.c)
target_link_libraries(chip8 glfw3)

if (WIN32)
	target_link_libraries(chip8 opengl32 openal gdi32)
else()
	target_link_libraries(chip8 GL openal)
endif()
