// (c) 2018 Zachary Wells
// This code is licensed under MIT license (see LICENSE for details)

#include "chip8.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#define PROGRAM_START 0x200

static uint8_t font[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(chip8_t *emulator) {
	memset(emulator->memory, 0, sizeof(emulator->memory));
	emulator->PC = PROGRAM_START;
	memcpy(emulator->font, font, 80);
}

void chip8_load_rom(chip8_t *emulator, const uint16_t *rom, size_t size) {
	chip8_init(emulator);
	memcpy(emulator->memory + PROGRAM_START, rom, size);
}

void chip8_cycle(chip8_t *emulator) {
	uint16_t opcode = emulator->memory[emulator->PC] << 8 | emulator->memory[emulator->PC + 1];
	#define DELAY (emulator->delay)
	#define SOUND (emulator->sound)
	#define PC  (emulator->PC)
	#define SP  (emulator->SP)
	#define I   (emulator->I)
	#define X   ((opcode & 0x0F00) >> 8)
	#define Y   ((opcode & 0x00F0) >> 4)
	#define VX  (emulator->V[X])
	#define VY  (emulator->V[Y])
	#define V0  (emulator->V[0x0])
	#define VF  (emulator->V[0xF])
	#define N   (opcode & 0x000F)
	#define NN  (opcode & 0x00FF)
	#define NNN (opcode & 0x0FFF)
	
	switch (opcode & 0xF000) {
		case 0x0000: {
			switch (opcode) {
				case 0x00E0: {
					memset(emulator->vram, 0, CHIP8_VRAM_SIZE);
					goto increment;
				} break;
				
				case 0x00EE: {
					PC = emulator->stack[--SP];
					goto increment;
				} break;
			}
			//call RCA 1802
		} break;
		
		case 0x1000: {
			PC = NNN;
			return;
		} break;
		
		case 0x2000: {
			emulator->stack[SP++] = PC;
			PC = NNN;
			return;
		} break;
		
		case 0x3000: {
			if (VX == NN) {
				PC += 2;
			}
			goto increment;
		} break;
		
		case 0x4000: {
			if (VX != NN) {
				PC += 2;
			}
			goto increment;
		} break;
		
		case 0x5000: {
			if (VX == VY) {
				PC += 2;
			}
			goto increment;
		} break;
		
		case 0x6000: {
			VX = NN;
			goto increment;
		} break;
		
		case 0x7000: {
			VX += NN;
			goto increment;
		} break;
		
		case 0x8000: {
			switch (opcode & 0xF) {
				case 0x0: {
					VX = VY;
					goto increment;
				} break;
				
				case 0x1: {
					VX = VX | VY;
					goto increment;
				} break;
				
				case 0x2: {
					VX = VX & VY;
					goto increment;
				} break;
				
				case 0x3: {
					VX = VX ^ VY;
					goto increment;
				} break;
				
				case 0x4: {
					uint16_t temp = (uint16_t)VX + (uint16_t)VY;
					VF = temp > 0xFF;
					VX = temp;
					goto increment;
				} break;
				
				case 0x5: {
					VF = VX >= VY;
					VX -= VY;
					goto increment;
				} break;
				
				case 0x6: {
					VF = VX & 1;
					VX >>= 1;
					goto increment;
				} break;
				
				case 0x7: {
					uint16_t temp = (uint16_t)VY - (uint16_t)VX;
					VF = !(temp > 0xFF);
					VX = temp;
					goto increment;
				} break;
				
				case 0xE: {
					VF = VX >> 7;
					VX <<= 1;
					goto increment;
				} break;
			}
		} break;
		
		case 0x9000: {
			if (VX != VY) {
				PC += 2;
			}
			goto increment;
		} break;
		
		case 0xA000: {
			I = NNN;
			goto increment;
		} break;
		
		case 0xB000: {
			PC = NNN + V0;
			return;
		} break;
		
		case 0xC000: {
			VX = (rand() % 0xFF) & NN;
		} break;
		
		case 0xD000: {
			VF = 0;
			
			for (uint16_t y = VY; y < (N + (uint16_t)VY); y++) {
				uint8_t *row = emulator->vram + y * 8;
				uint8_t texel = emulator->memory[I + (y - VY)];
				row += VX / 8;
				uint8_t byte0 = ((uint16_t)texel >> (VX % 8) & 0x00FF);
				uint8_t byte1 = ((uint16_t)texel << (8 - VX % 8) & 0x00FF);
				
				VF = (row[0] & byte0) || VF;
				row[0] ^= byte0;
				
				VF = (row[1] & byte1) || VF;
				row[1] ^= byte1;
			}
			
			goto increment;
		} break;
		
		case 0xE000: {
			switch (opcode & 0xFF) {
				case 0x9E: {
					if ((emulator->keypad.value >> VX) & 1) {
						PC += 2;
					}
					goto increment;
				} break;
				
				case 0xA1: {
					if (!((emulator->keypad.value >> VX) & 1)) {
						PC += 2;
					}
					goto increment;
				} break;
			} break;
		} break;
		
		case 0xF000: {
			switch (opcode & 0xFF) {
				case 0x07: {
					VX = DELAY;
					goto increment;
				} break;
				
				case 0x0A: {
					uint16_t keypad = emulator->keypad.value;
					uint16_t keypad_prev = emulator->keypad_previous.value;
					if (keypad != keypad_prev) {
						VX = 0;
						while(1) {
							if ((keypad & 1) != (keypad_prev & 1)) {
								break;
							}
							keypad >>= 1;
							keypad_prev >>= 1;
							VX++;
						}
						goto increment;
					}
					return;
				} break;
				
				case 0x15: {
					DELAY = VX;
					goto increment;
				} break;
				
				case 0x18: {
					SOUND = VX;
					goto increment;
				} break;
				
				case 0x1E: {
					I += VX;
					goto increment;
				} break;
				
				case 0x29: {
					I = (VX & 0xF) * 5;
					goto increment;
				} break;
				
				case 0x33: {
					emulator->memory[I + 0] = VX / 100 % 10;
					emulator->memory[I + 1] = VX / 10 % 10;
					emulator->memory[I + 2] = VX / 1 % 10;
					goto increment;
				} break;
				
				case 0x55: {
					memcpy(emulator->memory + I, emulator->V, X + 1);
					goto increment;
				} break;
				
				case 0x65: {
					memcpy(emulator->V, emulator->memory + I, X + 1);
					goto increment;
				} break;
			} break;
		} break;
	}
	
	increment:
	PC += 2;
	
	#undef DELAY
	#undef SOUND
	#undef PC
	#undef SP
	#undef I
}

void chip8_update(chip8_t *emulator, double deltaTime) {
	emulator->_timer += deltaTime;
	emulator->_cycle += deltaTime;
	
	double timerHz = 1.0 / 60.0;
	double clockHz = 1.0 / 500.0;
	
	while (emulator->_timer >= timerHz) {
		if (emulator->delay) {
			emulator->delay--;
		}
		if (emulator->sound) {
			emulator->sound--;
		}
		emulator->_timer -= timerHz;
	}
	
	while (emulator->_cycle >= clockHz) {
		chip8_cycle(emulator);
		emulator->keypad_previous.value = emulator->keypad.value;
		emulator->_cycle -= clockHz;
	}
}
