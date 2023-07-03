/*
 * CHIP-8
 * Emulator program
 *
 * Stefan Wong 2019
 */

#include <chrono>
#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

// For SDL (drawing)
#include <SDL2/SDL.h>
#include "chip8.hpp"

/*
 * Command line args
 */
struct c8_args
{
    size_t max_cycles;

    c8_args() : max_cycles(0) {}
};


static std::deque<std::pair<unsigned int, bool>> AudioQueue;

/*
 * Entry point
 */
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
    
    //// Screen to draw emulator onto
    //SDL_Window* window = SDL_CreateWindow(
    //        argv[1],
    //        SDL_WINDOWPOS_UNDEFINED,
    //        SDL_WINDOWPOS_UNDEFINED,
    //        W * 4,
    //        H * 6,
    //        SDL_WINDOW_RESIZABLE
    //);

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

    while(1)
    {
        int pitch;
        uint32_t* pixels;
        //uint32_t pixels[W * H];     // pixel memory for SDL display

        SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
        for(unsigned p = 0; p < (W * H); ++p)
            pixels[p] = p << 8;

        // Render display
        //cpu.renderTo(pixels, W, H);
        //SDL_UpdateTexture(texture, nullptr, pixels, W * sizeof(uint32_t));
        //SDL_UpdateTexture(texture, nullptr, pixels, pitch);
        SDL_UnlockTexture(texture);
        //SDL_UpdateTexture(texture, nullptr, pixels, 4 * W);
        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // quit if we press a key 
        for(SDL_Event ev; SDL_PollEvent(&ev);)
        {
            switch(ev.type)
            {
                case SDL_QUIT:
                case SDL_KEYDOWN:
                    goto cleanup;
            }
        }

    }


    // Init SDL audio
    //SDL_AudioSpec spec, obtained;
    //spec.freq     = 44100;
    //spec.format   = AUDIO_S16SYS;
    //spec.channels = 2;
    //spec.samples  = spec.freq / 20;     // 0.05 seconds of latency
    //spec.callback = [](void*, uint8_t* stream, int len)
    //{
    //    short* target = (short*) stream;
    //    while(len > 0 && !AudioQueue.empty())
    //    {
    //        auto& data = AudioQueue.front();
    //        for( ; len && data.first; target += 2, len -= 4, --data.first)
    //            target[0] = target[1] = data.second * 300 * ((len & 128) - 64);
    //        if(!data.first)
    //            AudioQueue.pop_front();
    //    }
    //};

    //SDL_OpenAudio(&spec, &obtained);
    //SDL_PauseAudio(0);

    // Setup the execution loop
    //unsigned int instr_per_frame = 50000;
    //int max_consecutive_instr = 2;         
    //int frames_done = 0;

    //// Operation loop
    //bool run = true;
    //auto start_time = std::chrono::system_clock::now();
    //while(run)
    //{
    //    // Process some instructions 
    //    for(int i = 0; i < max_consecutive_instr; ++i)
    //    {
    //        // TODO : keypress in wrong place
    //        if(cpu.getKeyPress())
    //            break;
    //        cpu.cycle();        
    //    }

    //    // handle keypresses
    //    //for(SDL_Event ev; SDL_PollEvent(&ev); )
    //    //{
    //    //    switch(ev.type)
    //    //    {
    //    //        case SDL_QUIT:
    //    //            run = false;
    //    //            break;

    //    //        case SDL_KEYDOWN:
    //    //        case SDL_KEYUP:
    //    //            auto i = keymap.find(ev.key.keysym.sym);
    //    //            if(i == keymap.end())
    //    //                break;
    //    //            if(i->second == -1)
    //    //            {
    //    //                run = false;
    //    //                break;
    //    //            }
    //    //            if(ev.type == SDL_KEYDOWN)
    //    //                cpu.updateKey(i->second, 1);
    //    //            else
    //    //                cpu.updateKey(i->second, 0);

    //    //            if(ev.type == SDL_KEYDOWN && cpu.getKeyPress())
    //    //                cpu.setKeyPress(i->second);

    //    //            break;
    //    //    }
    //    //}

    //    // Work out how many frames need to be rendered
    //    auto cur_time = std::chrono::system_clock::now();
    //    std::chrono::duration<float> elapsed_time = cur_time - start_time;
    //    int num_frames = int(elapsed_time.count() * 60) - frames_done;
    //    int pitch;      // expected to be 256 (64 pixels * 4 byte per pixel)

    //    if(num_frames > 0)
    //    {
    //        frames_done += num_frames;
    //        int st = int(cpu.getST()) - std::min(num_frames, int(cpu.getST()));
    //        int dt = int(cpu.getDT()) - std::min(num_frames, int(cpu.getDT()));

    //        // TODO: move into CPU structure
    //        cpu.setST(st);
    //        cpu.setDT(dt);

    //        uint32_t pixels[W * H];     // pixel memory for SDL display
    //        //SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
    //        for(unsigned p = 0; p < (W * H); ++p)
    //            pixels[p] = 0x0;

    //        // Render display
    //        //cpu.renderTo(pixels, W, H);
    //        SDL_UpdateTexture(texture, nullptr, pixels, W * sizeof(uint32_t));
    //        //SDL_UpdateTexture(texture, nullptr, pixels, pitch);
    //        //SDL_UnlockTexture(texture);
    //        //SDL_UpdateTexture(texture, nullptr, pixels, 4 * W);
    //        SDL_RenderClear(renderer);
    //        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    //        SDL_RenderPresent(renderer);


    //    
    //        // Render sound
    //        //SDL_LockAudio();
    //        //// TODO : turn audio off during debugging (as its quite irritating)
    //        //AudioQueue.emplace_back(obtained.freq * st / 60, false);
    //        //AudioQueue.emplace_back(obtained.freq * (num_frames - st) / 60, false);
    //        //SDL_UnlockAudio();
    //    }

    //    // Compensate for render speed 
    //    max_consecutive_instr = std::max(num_frames, 1) * instr_per_frame;
    //    if(cpu.getKeyPress() || !num_frames)
    //        SDL_Delay(1000 / 60);
    //}

cleanup:
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
