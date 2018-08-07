// (c) 2018 Zachary Wells
// This code is licensed under MIT license (see LICENSE for details)

// This is an example main file for using the chip8 interpreter.
// It is not required if you wish to use the interpreter somewhere else.
// This is the only source file which has dependencies.

#include <stdio.h>
#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>

#include "chip8.h"
static chip8_t emulator;

void load_rom(const char *fileName) {
	FILE *file = fopen(fileName, "rb");
	
	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		unsigned long length = ftell(file);
		rewind(file);
		uint16_t rom[0xFFF];
		fread(rom, 1, length, file);
		chip8_load_rom(&emulator, rom, length);
		fclose(file);
	} else {
		uint16_t no_rom[] = {
			0x0012
		};
		chip8_load_rom(&emulator, no_rom, sizeof(no_rom));
	}
}

void resize(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_REPEAT) {
		return;
	}
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		//quit
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	
	switch (key) {
		case GLFW_KEY_1: {
			emulator.keypad.K1 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_2: {
			emulator.keypad.K2 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_3: {
			emulator.keypad.K3 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_4: {
			emulator.keypad.KC = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_Q: {
			emulator.keypad.K4 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_W: {
			emulator.keypad.K5 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_E: {
			emulator.keypad.K6 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_R: {
			emulator.keypad.KD = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_A: {
			emulator.keypad.K7 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_S: {
			emulator.keypad.K8 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_D: {
			emulator.keypad.K9 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_F: {
			emulator.keypad.KE = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_Z: {
			emulator.keypad.KA = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_X: {
			emulator.keypad.K0 = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_C: {
			emulator.keypad.KB = (action == GLFW_PRESS);
		} return;
		case GLFW_KEY_V: {
			emulator.keypad.KF = (action == GLFW_PRESS);
		} return;
	}
}

void dragonDrop(GLFWwindow *window, int count, const char **paths) {
	if (count >= 1) {
		load_rom(paths[0]);
	}
}

int main(int argc, char **argv) {
	glfwInit();
	GLFWwindow *window = glfwCreateWindow(CHIP8_WIDTH * 8, CHIP8_HEIGHT * 9, "chip8", NULL, NULL);
	glfwSetWindowSizeCallback(window, resize);
	glfwSetDropCallback(window, dragonDrop);
	glfwSetKeyCallback(window, key);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(GLFW_TRUE);
	glEnable(GL_TEXTURE_2D);
	
	float index[] = {0.0, 1.0};
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, index);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, index);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, index);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, index);
	
	GLuint screen;
	glGenTextures(1, &screen);
	glBindTexture(GL_TEXTURE_2D, screen);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, CHIP8_WIDTH, CHIP8_HEIGHT, 0, GL_COLOR_INDEX, GL_BITMAP, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	ALCdevice  *al_device   = alcOpenDevice(NULL);
	ALCcontext *al_context = alcCreateContext(al_device, NULL);
	alcMakeContextCurrent(al_context);
	
	ALuint audio_source;
	alGenSources(1, &audio_source);
	
	alSourcef(audio_source, AL_PITCH, 2);
	alSourcef(audio_source, AL_GAIN, 0.2f);
	alSource3f(audio_source, AL_POSITION, 0, 0, 0);
	alSource3f(audio_source, AL_VELOCITY, 0, 0, 0);
	alSourcei(audio_source, AL_LOOPING, AL_FALSE);
	
	ALuint audio_buffer;
	alGenBuffers(1, &audio_buffer);
	alBufferData(audio_buffer, AL_FORMAT_MONO8, (uint8_t[]){
		0, 0, 0, 255, 255, 0, 0, 0
	}, 8, 1600);
	
	alSourcei(audio_source, AL_BUFFER, audio_buffer);
	
	if (argc >= 2) {
		load_rom(argv[1]);
	} else {
		load_rom(NULL);
	}
	
	glfwSetTime(0.0);
	double curTime, lastTime = glfwGetTime();
	while (glfwWindowShouldClose(window) != GLFW_TRUE) {
		glfwPollEvents();
		
		curTime = glfwGetTime();
		chip8_update(&emulator, curTime - lastTime);
		lastTime = curTime;
		
		if (emulator.sound) {
			alSourcei(audio_source, AL_LOOPING, AL_TRUE);
			alSourcePlay(audio_source);
		} else {
			alSourcei(audio_source, AL_LOOPING, AL_FALSE);
		}
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CHIP8_WIDTH, CHIP8_HEIGHT, GL_COLOR_INDEX, GL_BITMAP, emulator.vram);
		
		//immediate mode because I can't be bothered to make buffers and shaders for something so dead simple ;)
		glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex2i(-1, -1);
		glTexCoord2i(1, 1); glVertex2i(1, -1);
		glTexCoord2i(1, 0); glVertex2i(1, 1);
		glTexCoord2i(0, 0); glVertex2i(-1, 1);
		glEnd();
		
		glfwSwapBuffers(window);
	}
	
	alDeleteSources(1, &audio_source);
	alDeleteBuffers(1, &audio_buffer);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(al_context);
	alcCloseDevice(al_device);
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
