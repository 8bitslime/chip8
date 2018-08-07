// (c) 2018 Zachary Wells
// This code is licensed under MIT license (see LICENSE for details)

#include <stddef.h>
#include <stdint.h>

#define CHIP8_WIDTH  64
#define CHIP8_HEIGHT 32
#define CHIP8_VRAM_SIZE (CHIP8_WIDTH * CHIP8_HEIGHT / 8)

typedef union chip8_keypad_t {
	uint16_t value;
	struct {
		uint8_t K0 : 1;
		uint8_t K1 : 1;
		uint8_t K2 : 1;
		uint8_t K3 : 1;
		uint8_t K4 : 1;
		uint8_t K5 : 1;
		uint8_t K6 : 1;
		uint8_t K7 : 1;
		uint8_t K8 : 1;
		uint8_t K9 : 1;
		uint8_t KA : 1;
		uint8_t KB : 1;
		uint8_t KC : 1;
		uint8_t KD : 1;
		uint8_t KE : 1;
		uint8_t KF : 1;
	};
} chip8_keypad_t;

typedef struct chip8_t {
	union {
		uint8_t memory[4096];
		struct {
			uint8_t font[80];
			double _cycle;
			double _timer;
			uint16_t stack[16];
			uint8_t SP, delay, sound, V[16];
			uint16_t PC, I;
			chip8_keypad_t keypad;
			chip8_keypad_t keypad_previous;
			uint8_t vram[CHIP8_VRAM_SIZE];
		};
	};
} chip8_t;

void chip8_init(chip8_t *emulator);
void chip8_load_rom(chip8_t *emulator, const uint16_t *rom, size_t size);
void chip8_cycle(chip8_t *emulator);
void chip8_update(chip8_t *emulator, double deltaTime);
