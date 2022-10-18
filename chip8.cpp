#include "chip8.h"
#include <cstdlib>
#include <iostream>

Chip8::Chip8()
{
}

void Chip8::load_program(std::istream& is)
{
	uint32_t size = 0x1000 - 0x200;
	is.read((char*)&memory[0x200], size);
}

void Chip8::step()
{
	if (waiting_for_key) return;

	uint16_t opcode = (memory[pc++] << 8);
	opcode |= memory[pc++];

	uint8_t op = (opcode >> 12) & 0xF;
	uint16_t nnn = opcode & 0xFFF;
	uint8_t n = opcode & 0xF;
	uint8_t x = (opcode >> 8) & 0xF;
	uint8_t y = (opcode >> 4) & 0xF;
	uint8_t kk = opcode & 0xFF;

	switch (op)
	{
	case 0:
		if (opcode == 0x00E0) clear_display();
		else if (opcode == 0x00EE) pc = stack[sp--];
		break;
	case 1:
		pc = nnn;
		break;
	case 2:
		stack[++sp] = pc;
		pc = nnn;
		break;
	case 3:
		if (registers[x] == kk) pc += 2;
		break;
	case 4:
		if (registers[x] != kk) pc += 2;
		break;
	case 5:
		if (registers[x] == registers[y]) pc += 2;
		break;
	case 6:
		registers[x] = kk;
		break;
	case 7:
		registers[x] += kk;
		break;
	case 8:
		handle_alu(x, y, n);
		break;
	case 9:
		if (registers[x] != registers[y]) pc += 2;
		break;
	case 0xA:
		i = nnn;
		break;
	case 0xB:
		pc = registers[0] + nnn;
		break;
	case 0xC:
		registers[x] = rand() & kk;
		break;
	case 0xD:
		draw_sprite(registers[x], registers[y], n);
		break;
	case 0xE:
		if (kk == 0x9E && keypad[registers[x]]) pc += 2;
		else if (kk == 0xA1 && !keypad[registers[x]]) pc += 2;
		break;
	case 0xF:
		handle_graphics(x, kk);
		break;
	}
}

void Chip8::step_timers()
{
	if (delay_timer > 0) --delay_timer;
	if (sound_timer > 0) --sound_timer;
}

void Chip8::set_key(int key, bool pressed)
{
	keypad[key] = pressed;
	if (pressed && waiting_for_key)
	{
		registers[key_storage_register] = key;
		waiting_for_key = false;
	}
}

bool Chip8::should_play_sound() const
{
	return sound_timer > 0;
}

void Chip8::handle_alu(uint8_t x, uint8_t y, uint8_t n)
{
	switch (n)
	{
	case 0:
		registers[x] = registers[y];
		break;
	case 1:
		registers[x] |= registers[y];
		break;
	case 2:
		registers[x] &= registers[y];
		break;
	case 3:
		registers[x] ^= registers[y];
		break;
	case 4:
	{
		uint16_t r = registers[x] + registers[y];
		registers[0xF] = r > 255;
		registers[x] = r & 0xFF;
		break;
	}
	case 5:
		registers[0xF] = registers[x] > registers[y];
		registers[x] -= registers[y];
		break;
	case 6:
		registers[0xF] = registers[x] & 0x1;
		registers[x] >>= 1;
		break;
	case 7:
		registers[0xF] = registers[y] > registers[x];
		registers[x] = registers[y] - registers[x];
		break;
	case 0xE:
		registers[0xF] = registers[x] & 0x80;
		registers[x] <<= 1;
		break;
	}
}

void Chip8::handle_graphics(uint8_t x, uint8_t kk)
{
	switch (kk)
	{
	case 0x07:
		registers[x] = delay_timer;
		break;
	case 0x0A:
		waiting_for_key = true;
		key_storage_register = x;
		break;
	case 0x15:
		delay_timer = registers[x];
		break;
	case 0x18:
		sound_timer = registers[x];
		break;
	case 0x1E:
		i += registers[x];
		break;
	case 0x29:
		i = registers[x] * 5;
		break;
	case 0x33:
	{
		uint8_t r = registers[x];
		for (int j = 2; j >= 0; --j)
		{
			memory[i + j] = r % 10;
			r /= 10;
		}
		break;
	}
	case 0x55:
		for (int j = 0; j <= x; ++j)
			memory[i + j] = registers[j];
		break;
	case 0x65:
		for (int j = 0; j <= x; ++j)
			registers[j] = memory[i + j];
		break;
	}
}

void Chip8::draw_sprite(uint8_t x, uint8_t y, uint8_t n)
{
	bool cleared = false;
	for (int row = 0; row < n; ++row)
	{
		uint8_t sprite = memory[i + row];
		for (int col = 0; col < 8; ++col)
		{
			int xx = (x + col) % DISPLAY_WIDTH;
			int yy = (y + row) % DISPLAY_HEIGHT;

			bool sprite_pixel = (sprite >> (8 - col)) & 1;
			bool &pixel = display[yy * DISPLAY_WIDTH + xx];
			if (!cleared)
			{
				cleared = sprite_pixel && pixel;
			}
			pixel ^= sprite_pixel;
		}
	}
	registers[0xF] = cleared;
}

void Chip8::clear_display()
{
	for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i)
		display[i] = false;
}

const bool* Chip8::get_display() const
{
	return display;
}