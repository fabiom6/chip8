#pragma once
#include <cstdint>
#include <istream>



class Chip8
{
public:
	static const int DISPLAY_WIDTH = 64;
	static const int DISPLAY_HEIGHT = 32;

private:
	uint8_t memory[0x1000] = {
		#include "sprites.h"
	};
	uint8_t registers[0x10] = { 0 };
	uint16_t i = 0;

	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;

	uint16_t pc = 0x200;
	uint8_t sp = 0;

	uint16_t stack[16];

	bool waiting_for_key = false;
	uint8_t key_storage_register = 0;

	bool display[DISPLAY_WIDTH * DISPLAY_HEIGHT] = { false };
	bool keypad[16] = { false };

	void handle_alu(uint8_t x, uint8_t y, uint8_t n);

	void handle_graphics(uint8_t x, uint8_t kk);
	void draw_sprite(uint8_t x, uint8_t y, uint8_t n);
	void clear_display();


public:

	Chip8();

	void load_program(std::istream& is);

	void step();
	void step_timers();

	void set_key(int key, bool pressed);
	bool should_play_sound() const;

	const bool* get_display() const;
};