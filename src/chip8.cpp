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

    // Top line 
    oss << "pc   I     flags    " << std::endl;
    oss << "0x" << std::hex << std::setw(4) << std::setfill('0') << this->pc << " ";
    oss << "0x" << std::hex << std::setw(4) << std::setfill('0') << this->I << " ";

    return oss.str();
}

/*
 * C8Exec
 * Chip-8 execution context. Allows saving and resuming
 * of pipeline state.
 */
C8Exec::C8Exec()
{
    this->u   = 0;
    this->p   = 0;
    this->x   = 0;
    this->y   = 0;
    this->kk  = 0;
    this->nnn = 0;
}

C8Exec::~C8Exec() {} 

C8Exec::C8Exec(const C8Exec& that)
{
    this->u   = that.u;
    this->p   = that.p;
    this->x   = that.x;
    this->y   = that.y;
    this->kk  = that.kk;
    this->nnn = that.nnn;
}

/*
 * Chip8
 * Chip-8 object
 */
Chip8::Chip8()
{
    this->mem_size = 4096;
    this->alloc_mem();
    this->init_mem();
    this->state.pc = CHIP8_START_ADR;
}

Chip8::~Chip8()
{
    delete[] this->mem;
}

Chip8::Chip8(const Chip8& that)
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

void Chip8::init_mem(void)
{
    for(unsigned int m = 0; m < this->mem_size; ++m)
        this->mem[m] = 0;
}

/*
 * cycle()
 * Run through the instruction cycle 
 */
void Chip8::cycle(void)
{
    uint16_t op;
    uint8_t vx, vy;
    uint8_t kk;

    // decode 
    this->cur_opcode = (this->mem[this->state.pc] << 8) |  
                       (this->mem[this->state.pc + 1]);
    op = this->cur_opcode & 0xF00F;
    // execute
    switch(op)
    {
        case C8_ADDVx:
            vx = (op & 0x0F00) >> 8;
            kk = (op & 0x00F0) >> 4;
            this->state.V[vx] += kk;
            break;
        case C8_LD:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vy];
            break;

        case C8_OR:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] | this->state.V[vy];
            break;

        case C8_AND:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] & this->state.V[vy];
            break;

        case C8_XOR:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] ^ this->state.V[vy];
            break;

        case C8_ADD:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] + this->state.V[vy];
            break;

        case C8_SUB:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.V[vx] = this->state.V[vx] - this->state.V[vy];
            break;

        case C8_SHR:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.VF = (vx & 0x01) ? 0x0001 : 0x000;
            this->state.V[vx] = this->state.V[vx] >> 1;
            break;

        case C8_SHL:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            this->state.VF = (vx & 0x01) ? 0x0001 : 0x000;
            this->state.V[vx] = this->state.V[vx] << 1;
            break;

        case C8_SNE:
            vx = (op & 0x0F00) >> 8;
            vy = (op & 0x00F0) >> 4;
            if(this->state.V[vx] != this->state.V[vy])
                this->state.pc += 2;
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

std::vector<uint8_t> Chip8::dumpMem(void) const
{
    std::vector<uint8_t>dump(this->mem_size);
    for(unsigned int m = 0; m < this->mem_size; ++m)
        dump[m] = this->mem[m];

    return dump;
}
