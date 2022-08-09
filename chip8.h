#pragma once
#include <cstdint>

class Chip8
{
	uint8_t memory[0x1000] = {0};
	uint8_t registers[0x10] = {0};
	uint16_t i = 0;

	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;

	uint16_t pc;
	uint8_t sp;

	uint16_t stack[16];

	bool display[64 * 32] = {false};

	void clear_display();
	void handle_alu(uint8_t x, uint8_t y, uint8_t n);
	void draw_sprite(uint8_t x, uint8_t y, uint8_t n);
	void handle_graphics(uint8_t x, uint8_t kk);

public:
	static const int CLOCK_SPEED = 500;
	static const int TIMER_SPEED = 60;

	Chip8();

	void step();
	void step_timers();
};