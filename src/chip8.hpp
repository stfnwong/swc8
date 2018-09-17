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
#define C8_ZERO   0x0000 // special case here for zeros

#define C8_JP     0x1000
#define C8_CALL   0x2000
#define C8_SEI    0x3000
#define C8_SNEI   0x4000
#define C8_SE     0x5000
#define C8_LDI    0x6000
#define C8_ADDI   0x7000
#define C8_LD     0x8000
#define C8_OR     0x8001
#define C8_AND    0x8002
#define C8_XOR    0x8003
#define C8_ADD    0x8004
#define C8_SUB    0x8005
#define C8_SHR    0x8006
#define C8_SUBN   0x8007
#define C8_SHL    0x800E
#define C8_SNE    0x9000
#define C8_LDII   0xA000
#define C8_JPV    0xB000
#define C8_RND    0xC000
#define C8_DRW    0xD000
#define C8_SKP    0xE09E
#define C8_SKNP   0xE0A1
#define C8_LDDTVX 0xF007
#define C8_LDVXDT 0xF015
#define C8_LDK    0xF00A
#define C8_LDB    0xF033
#define C8_LDIVX  0xF055
#define C8_LDVXI  0xF065

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
        void init(void);
        // Print format
        std::string toString(void) const;
        std::string diffStr(const C8Proc& that);
        bool operator==(const C8Proc& that)
        {
            for(int r = 0; r < 16; ++r)
            {
                if(this->V[r] != that.V[r])
                    return false;
            }
            if(this->pc != that.pc)
                return false;
            if(this->sp != that.sp)
                return false;
            if(this->I != that.I)
                return false;
            if(this->dt != that.dt)
                return false;
            if(this->st != that.st)
                return false;
            for(int s = 0; s < 12; s++)
            {
                if(this->stack[s] != that.stack[s])
                    return false;
            }

            return true;
        }
};

// Thin wrapper around a vector of processor states 
class C8StateLog
{
    private: 
        unsigned int max_log;
        unsigned int log_ptr;
        std::vector<C8Proc> log;

    public:
        C8StateLog();
        ~C8StateLog();
        C8StateLog(const C8StateLog& that);
        void add(const C8Proc& state);
        C8Proc get(const unsigned int idx) const;
        std::vector<C8Proc> getLog(void) const;
};


// TODO: this class seems unweildy..
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
        bool log_state;
        C8StateLog state_log;
        C8Proc state;

    private:
        // Execution context
        C8Exec exec;
        bool log_exec;
        std::vector <C8Exec> exec_log;

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

    private:
        // internal execution
        void exec_zero_op(void);

    public:
        Chip8();
        ~Chip8();
        Chip8(const Chip8& that);

        void cycle(void);
        /*
         * loadMem()
         * Load an object file into memory at at a particular offset
         */
        int loadMem(const std::string& filename, int offset);
        /*
         * loadMem()
         * Load a vector of arbitrary bytes directly into memory.
         * Intended mainly for testing.
         */
        void loadMem(const std::vector<uint8_t>& data, int offset);
        /* 
         * dumpMem()
         * Dump the memory contents into a vector. 
         */
        std::vector<uint8_t> dumpMem(void) const;
        /*
         * readMem()
         * Read a single byte from memory
         */
        uint8_t readMem(const unsigned int addr) const;

        /*
         * setTrace()
         * Turn the state tracer on or off
         */
        void setTrace(const bool v);

        /* 
         * getTrace()
         * Return the state trace as a vector 
         */
        std::vector<C8Proc> getTrace(void);
};


#endif /*__CHIP_8_HPP*/
