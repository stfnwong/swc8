/* CHIP-8
 * A Chip-8 object for emulating the Chip-8 VM
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "chip8.hpp"

/*
 * C8Proc
 * Chip-8 processor state
 */
C8Proc::C8Proc() {}
C8Proc::~C8Proc() {}

C8Proc::C8Proc(const C8Proc& that)
{
    for(int r = 0; r < 16; r++)
        this->V[r] = that.V[r];
    this->pc = that.pc;
    this->I  = that.I;
}



std::string C8Proc::toString(void) const
{
    std::ostringstream oss;


    return oss.str();
}

/*
 * Chip8
 * Chip-8 object
 */
Chip8::Chip8()
{
    this->mem_size = 4096;
    this->alloc_mem();
}

Chip8::~Chip8()
{
    delete[] this->mem;
}

Chip8::~Chip8(const Chip8& that)
{
    this->state    = that.state;
    this->mem_size = that.mem_size;
    this->alloc_mem();
    for(int m = 0; m < this->mem_size; m++)
        this->mem[m] = that.mem[m];
}

void Chip8::alloc_mem(void)
{
    this->mem = new uint8_t[this->mem_size];
}

/*
 * cycle()
 * Run through the instruction cycle 
 */
void Chip8::cycle(void)
{
    uint16_t op;
    uint8_t vx, vy;

    this->cur_opcode = (this->mem[this->state.pc] << 8) |  
                       (this->mem[this->state.pc + 1]);
    op = this->cur_opcode & 0xF00F;
    switch(op)
    {
        case C8_LDVxVy:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vy];
            break;

        case C8_ORVxVy:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] | this->state.V[vy];
            break;

        case C8_ANDVxVy:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] & this->state.V[vy];
            break;

        case C8_XORVxVy:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] ^ this->state.V[vy];
            break;

        case C8_ADDVxVy:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] + this->state.V[vy];
            break;

        case C8_SUBVxVy:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] - this->state.V[vy];
            break;

        default:
            std::cout << "[" << __FUNCTION__ << "] invalid op <0x" 
                << std::hex << std::setw(4) << std::setfill('0') 
                << this->cur_opcode << std::endl;
            break;
    }

    this->state.pc += 2;
}



int Chip8::loadMem(const std::string& filename, int offset)
{
    int status = 0;
    int num_bytes = 0;
    std::ifstream infile(filename, std::ios::binary);

    infile.seekg(0, std::ios::end);
    num_bytes = infile.tellg();
    infile.seekg(0, std::ios::beg);

    try{
        infile.read((char*) &this->mem[offset], sizeof(uint8_t) * num_bytes);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __FUNCTION__ << "] caught execption [%s]" << 
            e.what() << std::endl;
        status = -1;
    }

    return status;
}

