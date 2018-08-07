# Chip8 interpreter in C
Just a simple Chip8 interpreter written in C11. I made it for learning purposes, so hopefully others can learn from this source code as well.

The interpreter itself is entirely self contained and has no dependencies (other than the C standard library). The actual rendering and audio is handled outside of the interpreter by [GLFW](https://github.com/glfw/glfw) and [OpenAL](https://www.openal.org/) respectively.

# Building
Simply build it with CMake, for example:  
`$ mkdir build`  
`$ cd build`  
`$ cmake ..`  
`$ cmake --build .`  
This produces an executable called `chip8` within the build directory. You can either launch it with chip8 programs from the command line, or drag and drop roms on top of the window.

# Dependencies
You must have [GLFW](https://github.com/glfw/glfw) and [OpenAL](https://www.openal.org/) installed to build and run the main source file. However, just chip8.h and chip8.c can be built without any libraries at all.
