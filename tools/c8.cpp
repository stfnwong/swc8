/*
 * CHIP-8
 * Emulator program
 *
 * Stefan Wong 2019
 */

#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

// For SDL (drawing)
#include <SDL2/SDL.h>
#include "chip8.hpp"

static std::deque<std::pair<unsigned int, bool>> AudioQueue;

int main(int argc, char *argv[])
{
    constexpr const unsigned W = 64;
    constexpr const unsigned H = 32;

    if(argc < 2)
    {
        std::cout << "No input file specified, exiting" << std::endl;
        return -1;
    }

    int status;
    Chip8 cpu;
    status = cpu.loadMem(std::string(argv[1]), 0x200);
    if(status != 0)
    {
        std::cerr << "Failed to load memory contents from file [" << std::string(argv[1]) << "]" << std::endl;
        return -1;
    }
    
    // Screen to draw emulator onto
    SDL_Window* window = SDL_CreateWindow(
            argv[1],
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            W * 4,
            H * 6,
            SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture*  texture  = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            W,
            H
    );

    // Map the SDL keyboard symbols to Chip-8 keypad codes
    std::unordered_map<int, int> keymap {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
        {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
        {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
        {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF},
        {SDLK_5, 0x5}, {SDLK_6, 0x6}, {SDLK_7, 0x7}, 
        {SDLK_8, 0x8}, {SDLK_9, 0x9}, {SDLK_0, 0x0}, {SDLK_ESCAPE, -1}
    };

    // Init SDL audio
    SDL_AudioSpec spec, obtained;
    spec.freq     = 44100;
    spec.format   = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples  = spec.freq / 20;     // 0.05 seconds of latency
    spec.callback = [](void*, uint8_t* stream, int len)
    {
        short* target = (short*) stream;
        while(len > 0 && !AudioQueue.empty())
        {

            auto& data = AudioQueue.front();
            for( ; len && data.first; target += 2, len -= 4, --data.first)
                target[0] = target[1] = data.second * 300 * ((len & 128) - 64);
            if(!data.first)
                AudioQueue.pop_front();
        }
    };

    SDL_OpenAudio(&spec, &obtained);
    SDL_PauseAudio(0);

    return 0;
}
