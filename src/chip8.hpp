/* CHIP-8
 * A Chip-8 object for emulating the Chip-8 VM
 *
 * Stefan Wong 2018
 */

#ifndef __CHIP_8_HPP
#define __CHIP_8_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <random>
#include "opcode.hpp"

#define CHIP8_START_ADR 0x200
// TODO: fontset

// Chip8 instruction codes 
// effectively a kind of opcode mask 
#define C8_JP    0x1000
#define C8_CALL  0x2000
#define C8_SEI   0x3000
#define C8_SNEI  0x4000
#define C8_SE    0x5000
#define C8_LDI   0x6000
#define C8_ADDI  0x7000
#define C8_LD    0x8000
#define C8_OR    0x8001
#define C8_AND   0x8002
#define C8_XOR   0x8003
#define C8_ADD   0x8004
#define C8_SUB   0x8005
#define C8_SHR   0x8006
#define C8_SUBN  0x8007
#define C8_SHL   0x800E
#define C8_SNE   0x9000
#define C8_LDII  0xA000
#define C8_JPV   0xB000
#define C8_RND   0xC000
#define C8_DRW   0xD000
#define C8_SKP   0xE09E
#define C8_SKNP  0xE0A1
#define C8_LDDT  0xF007
#define C8_LDK   0xF00A

// TODO : handle zero codes seperately

/*
 * C8Proc
 * Chip-8 processor state
 */
class C8Proc
{
    public:
        // Internals
        uint8_t  V[16];
        uint16_t pc;            // program counter
        uint16_t sp;            // stack pointer 
        // special registers 
        uint16_t VF;
        uint16_t I;
        // Timers 
        uint8_t dt;             // Delay timer
        uint8_t st;             // Sound timer
        // Stack 
        uint16_t stack[12];     // stack memory
        // keys 
        uint8_t  keys[16];      // key input

    public:
        C8Proc();
        ~C8Proc();
        C8Proc(const C8Proc& that);
        // Print format
        std::string toString(void) const;
};

/*
 * C8Exec
 * Chip-8 execution context. Allows saving and resuming
 * of pipeline state.
 */
class C8Exec
{
    public:
        uint8_t u;
        uint8_t p;
        uint8_t vx;
        uint8_t vy;
        uint8_t kk;
        uint16_t nnn;

    public:
        C8Exec();
        ~C8Exec();
        C8Exec(const C8Exec& that);
        std::string toString(void);
};

/*
 * Chip8
 * Chip-8 object
 */
static constexpr const unsigned int W = 64;
static constexpr const unsigned int H = 64;

class Chip8
{
    private:
        // Processor state 
        C8Proc state;

    private:
        // Execution context
        C8Exec exec;

    private:
        // Display memory 
        uint8_t disp_mem[W * H / 8];
        uint8_t fontmem[16 * 5];

    public:
        // Memory 
        uint8_t mem[0x1000];
        uint16_t mem_size;
        void init_mem(void);

    private:
        uint16_t cur_opcode;
        uint16_t cur_addr;

    public:
        Chip8();
        ~Chip8();
        Chip8(const Chip8& that);

        void cycle(void);
        int loadMem(const std::string& filename, int offset);
        /* 
         * dumpMem()
         * Dump the memory contents into a vector. 
         */
        std::vector<uint8_t> dumpMem(void) const;
};


#endif /*__CHIP_8_HPP*/
