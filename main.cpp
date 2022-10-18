#include <iostream>
#include <fstream>
#include <unordered_map>
#include <SDL.h>

#include "chip8.h"
#include "sound.h"
#include "filechooser.h"

static const int CLOCK_SPEED = 1000;
static const int TIMER_SPEED = 60;

std::unordered_map<int, int> keymap = {
	{SDL_SCANCODE_1, 1},   {SDL_SCANCODE_2, 2},   {SDL_SCANCODE_3, 3},   {SDL_SCANCODE_4, 0xC},
	{SDL_SCANCODE_Q, 4},   {SDL_SCANCODE_W, 5},   {SDL_SCANCODE_E, 6},   {SDL_SCANCODE_R, 0xD},
	{SDL_SCANCODE_A, 7},   {SDL_SCANCODE_S, 8},   {SDL_SCANCODE_D, 9},   {SDL_SCANCODE_F, 0xE},
	{SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0},   {SDL_SCANCODE_C, 0xB}, {SDL_SCANCODE_V, 0xF},
};

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Failed to initialize SDL!\n";
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_HIDDEN);
	if (!window)
	{
		std::cerr << "Failed to create window!\n";
		SDL_Quit();
		return -2;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		std::cerr << "Failed to create renderer!\n";
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -3;
	}

	SDL_Texture* screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	if (!screen)
	{
		std::cerr << "Failed to create screen texture!\n";
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -4;
	}

	Sound sound;
	bool playing_sound = false;

	std::ifstream file;
	FileChooser chooser;
	chooser.get_file_from_user(file);

	SDL_ShowWindow(window);

	Chip8 chip;
	chip.load_program(file);
	file.close();

	bool running = true;

	long update_every = 1000 / CLOCK_SPEED;
	long update_timer_every = 1000 / TIMER_SPEED;

	long elapsed = 0;
	long timer_elapsed = 0;

	long start = SDL_GetTicks64();
	while (running)
	{
		long start = SDL_GetTicks64();
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT) running = false;
			else if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP)
			{
				if (keymap.find(ev.key.keysym.scancode) != keymap.end())
				{
					int key = keymap[ev.key.keysym.scancode];
					chip.set_key(key, ev.type == SDL_KEYDOWN);
				}
			}
		}

		int pitch;
		uint32_t* pixels = nullptr;
		const bool* display = chip.get_display();

		SDL_LockTexture(screen, nullptr, (void**)&pixels, &pitch);
		for (int row = 0; row < Chip8::DISPLAY_HEIGHT; ++row)
		{
			for (int col = 0; col < Chip8::DISPLAY_WIDTH; ++col)
			{
				uint32_t color = display[row * Chip8::DISPLAY_WIDTH + col] ? 0xFF00AA00 : 0xFF000000;
				pixels[row * Chip8::DISPLAY_WIDTH + col] = color;
			}
		}
		SDL_UnlockTexture(screen);
		SDL_RenderCopy(renderer, screen, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		long end = SDL_GetTicks64();
		elapsed += end - start;
		timer_elapsed += end - start;
		start = end;

		while (elapsed >= update_every)
		{
			chip.step();
			if (chip.should_play_sound() && !playing_sound)
			{
				playing_sound = true;
				sound.play();
			}
			else if (!chip.should_play_sound())
			{
				playing_sound = false;
				sound.stop();
			}

			elapsed -= update_every;
		}

		while (timer_elapsed >= update_timer_every)
		{
			chip.step_timers();
			timer_elapsed -= update_timer_every;
		}
	}

	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}