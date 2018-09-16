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
C8Proc::C8Proc()
{
    for(int r = 0; r < 16; r++)
        this->V[r] = 0;
    this->pc = 0;
    this->sp = 0;
    // special registers 
    this->VF = 0;
    this->I  = 0;
    // timers 
    this->dt = 0;
    this->st = 0;
}


C8Proc::~C8Proc() {}

C8Proc::C8Proc(const C8Proc& that)
{
    for(int r = 0; r < 16; r++)
        this->V[r] = that.V[r];
    this->pc = that.pc;
    this->I  = that.I;
    this->dt = that.dt;
    this->st = that.st;
    for(int s = 0; s < 12; s++)
        this->stack[s] = that.stack[s];
    for(int k = 0; k < 16; k++)
        this->keys[k] = that.keys[k];
}

std::string C8Proc::toString(void) const
{
    std::ostringstream oss;

    // Top line 
    oss << "pc   I     flags    " << std::endl;
    oss << "0x" << std::hex << std::setw(4) << std::setfill('0') << this->pc << " ";
    oss << "0x" << std::hex << std::setw(4) << std::setfill('0') << this->I << " ";
    // TODO : how to print stack?

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
    this->vx  = 0;
    this->vy  = 0;
    this->kk  = 0;
    this->nnn = 0;
}

C8Exec::~C8Exec() {} 

C8Exec::C8Exec(const C8Exec& that)
{
    this->u   = that.u;
    this->p   = that.p;
    this->vx  = that.vx;
    this->vy  = that.vy;
    this->kk  = that.kk;
    this->nnn = that.nnn;
}

/*
 * Chip8
 * Chip-8 object
 */
Chip8::Chip8()
{
    this->mem_size = 0x1000;
    this->init_mem();
}

Chip8::~Chip8() {} 

Chip8::Chip8(const Chip8& that)
{
    this->state    = that.state;
    this->mem_size = that.mem_size;
    for(int m = 0; m < this->mem_size; m++)
        this->mem[m] = that.mem[m];
}

void Chip8::init_mem(void)
{
    for(unsigned int m = 0; m < this->mem_size; ++m)
        this->mem[m] = 0;
    // insert JP 0x200 instruction at start of memory
    this->mem[0] = 0x12;
    this->mem[1] = 0x00;
    // TODO : place  simulator internals at start of memory ?
}

/*
 * cycle()
 * Run through the instruction cycle 
 */
void Chip8::cycle(void)
{
    uint16_t instr_code;
    // decode 
    this->cur_opcode = (this->mem[this->state.pc] << 8) |  
                       (this->mem[this->state.pc + 1]);
    this->state.pc += 2;

    this->exec.u  =  (this->cur_opcode >> 12) & 0x0F;
    this->exec.p  =  (this->cur_opcode      ) & 0x0F;
    this->exec.vx =  (this->cur_opcode >> 8 ) & 0x0F;
    this->exec.vy =  (this->cur_opcode >> 4 ) & 0x0F;
    this->exec.kk =  (this->cur_opcode      ) & 0xFF;
    this->exec.nnn = (this->cur_opcode      ) & 0x0FFF;

    instr_code = 0x0000 | (this->exec.u << 12) | (this->exec.p);
    switch(instr_code)
    {
        case C8_JP:
            this->state.pc = this->exec.nnn;
            break;

        case C8_CALL:
            this->state.sp++;
            this->state.stack[this->state.sp % 12] = this->state.pc;
            this->state.pc = this->exec.nnn;
            break;

        case C8_SEI:    // LD Vx, kk
            this->state.V[this->exec.vx] = this->exec.kk;
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
    std::vector<uint8_t> mem_out(this->mem_size);
    for(unsigned int m = 0; m < this->mem_size; ++m)
        mem_out[m] = this->mem[m];
        //mem_out.push_back(this->mem[m]);

    return mem_out;
}

uint8_t Chip8::readMem(const unsigned int addr) const
{
    return this->mem[addr];
}
