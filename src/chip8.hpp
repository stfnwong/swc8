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

#define CHIP8_START_ADR 0x200
// TODO: fontset

// Chip-8 opcodes
#define C8_JMP      0x1000
#define C8_CALL     0x2000
#define C8_ADDVx    0x7000
#define C8_LDVxVy   0x8000
#define C8_ORVxVy   0x8001
#define C8_ANDVxVy  0x8002
#define C8_XORVxVy  0x8003
#define C8_ADDVxVy  0x8004
#define C8_SUBVxVy  0x8005
#define C8_SHRVxVy  0x8006
#define C8_SUBNVxBy 0x8007
#define C8_SHLVxVy  0x800E
#define C8_SNEVxVy  0x9000

/*
 * C8Proc
 * Chip-8 processor state
 */
class C8Proc
{
    public:
        // Internals
        uint8_t  V[16];
        uint16_t VF;
        uint16_t I;
        uint16_t pc;
        uint16_t sp;

    public:
        C8Proc();
        ~C8Proc();
        C8Proc(const C8Proc& that);
        // Print format
        std::string toString(void) const;
};

/*
 * Chip8
 * Chip-8 object
 */
class Chip8
{
    private:
        C8Proc state;
        uint8_t* mem;
        uint16_t mem_size;
        void alloc_mem(void);
        void init_mem(void);

    private:
        uint16_t cur_opcode;

    public:
        Chip8();
        ~Chip8();
        Chip8(const Chip8& that);

        void cycle(void);
        int loadMem(const std::string& filename, int offset);
        std::vector<uint8_t> dumpMem(void) const;

};



#endif /*__CHIP_8_HPP*/
