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
    this->init();
}

C8Proc::~C8Proc() {}

C8Proc::C8Proc(const C8Proc& that)
{
    for(int r = 0; r < 16; r++)
        this->V[r] = that.V[r];
    this->pc = that.pc;
    this->sp = that.sp;
    this->I  = that.I;
    this->dt = that.dt;
    this->st = that.st;
    for(int s = 0; s < 12; s++)
        this->stack[s] = that.stack[s];
    for(int k = 0; k < 16; k++)
        this->keys[k] = that.keys[k];
}

void C8Proc::init(void)
{
    for(int r = 0; r < 16; r++)
        this->V[r] = 0;
    for(int s = 0; s < 12; ++s)
        this->stack[s] = 0;
    for(int k = 0; k < 16; ++k)
        this->keys[k] = 0;
    this->pc = 0;
    this->sp = 0;
    // special registers 
    this->I  = 0;
    // timers 
    this->dt = 0;
    this->st = 0;
}

std::string C8Proc::toString(void) const
{
    std::ostringstream oss;
    
    oss << "---------------------------------------------------------------------" << std::endl;
    // Program counter, Stack pointer, I register
    oss << " [PC]    [SP]   [I]     " << std::endl;
    oss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << this->pc << "  ";
    oss << " 0x" << std::hex << std::setw(2) << std::setfill('0') << this->sp << "   ";
    oss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << this->I << " ";

    // Make a little 'stack diagram that goes next to a 'register' 
    // diagram and print that line by line 
    oss << std::endl << std::endl;
    oss << " Stack           Registers " << std::endl;
    for(int r = 0; r < 16; ++r)
    {
        // stack
        if(r < 12)
        {
            oss << " " << std::dec << std::setw(2) << r;
            oss << " [ 0x" << std::hex << std::setw(4) << std::setfill('0') 
                << unsigned(this->stack[r]) << " ] ";
        }
        else if(r == 13)
        {
            oss << " ST: [ 0x" << std::hex << std::setw(2) 
                << std::setfill('0') << unsigned(this->st) << " ]"; 
            oss << "  ";
        }
        else if(r == 14)
        {
            oss << " DT: [ 0x" << std::hex << std::setw(2)
                << std::setfill('0') << unsigned(this->dt) << " ]"; 
            oss << "  ";
        }
        else 
            oss << std::setw(15) << std::setfill(' ') << " ";

        // registers 
        oss << "V" << std::hex << std::setw(1) << std::setfill(' ') 
            << std::uppercase << r;
        oss << " [ 0x" << std::hex << std::setw(2) << std::setfill('0') 
                << unsigned(this->V[r]) << " ] ";
        oss << std::endl;
    }

    return oss.str();
}

std::string C8Proc::diffStr(const C8Proc& that)
{
    std::ostringstream oss;

    oss << "\0";
    // check registers 
    for(int r = 0; r < 16; ++r)
    {
        if(this->V[r] != that.V[r])
            oss << "this->V" << std::hex << std::uppercase << r 
                << " [" << std::hex << std::setw(2) << std::setfill('0')
                << unsigned(this->V[r]) << "] != that.V" << std::hex << r
                << " [" << std::hex << std::setw(2) << std::setfill('0')
                << unsigned(that.V[r]) << "]" << std::endl;
    }
    // check stack
    for(int s = 0; s < 12; ++s)
    {
        if(this->stack[s] != that.stack[s])
        {
            oss << "this->stack[" << std::hex << std::uppercase << s 
                << "] (" << std::hex << std::setw(2) << std::setfill('0')
                << unsigned(this->stack[s]) << ") != that.stack[" 
                << std::hex << s << "] [" << std::hex << std::setw(2) 
                << std::setfill('0') << unsigned(that.stack[s]) 
                << "]" << std::endl;
        }
    }
    // rest of state 
    if(this->pc != that.pc)
    {
        oss << "this->pc [" << std::hex << std::setw(4) 
            << std::setfill('0') << unsigned(this->pc) 
            << "] != that.pc [" << std::setw(4) 
            << std::setfill('0') << unsigned(that.pc)
            << "]" << std::endl;
    }
    if(this->sp != that.sp)
    {
        oss << "this->sp [" << std::hex << std::setw(4) 
            << std::setfill('0') << unsigned(this->sp) 
            << ") != that.sp [" << std::setw(4) << std::setfill('0') 
            << unsigned(that.sp) << "]" << std::endl;
    }
    if(this->I != that.I)
    {
        oss << "this->I [" << std::hex << std::setw(4) 
            << std::setfill('0') << unsigned(this->I) 
            << "] != that.I [" << std::setw(4) 
            << std::setfill('0') << unsigned(that.I)
            << "]" << std::endl;
    }
    //if(this->dt != that.dt)
    //{
    //    oss << "this->dt (" << std::hex << std::setw(4) 
    //        << std::setfill('0') << this->dt << ") != that.dt ("
    //        << std::setw(4) << std::setfill('0') << that.dt 
    //        << ")" << std::endl;
    //}

    return oss.str();
}

bool C8Proc::operator==(const C8Proc& that)
{
    // registers
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

    // stack
    for(int s = 0; s < 12; s++)
    {
        if(this->stack[s] != that.stack[s])
            return false;
    }

    return true;
}

bool C8Proc::operator!=(const C8Proc& that)
{
    return (*this == that);
}

/*
 * C8StateLog()
 */
C8StateLog::C8StateLog() 
{
    this->max_log = 32;
    this->log_ptr = 0;
    this->log = std::vector<C8Proc>(this->max_log);
}
C8StateLog::~C8StateLog() {} 

C8StateLog::C8StateLog(const C8StateLog& that)
{
    this->max_log = that.max_log;
    this->log_ptr = 0;
    this->log = std::vector<C8Proc>(this->max_log);
    for(unsigned int l = 0; l < this->max_log; ++l)
        this->log[l] = that.log[l];
}

void C8StateLog::add(const C8Proc& state)
{
    if(this->log_ptr >= this->max_log-1)
        this->log_ptr = 0;
    this->log[this->log_ptr] = state;
    this->log_ptr++;
}

C8Proc C8StateLog::get(const unsigned int idx) const
{
    if(idx >= 0 && idx < this->max_log)
        return this->log[idx];
    
    C8Proc p;
    return p;
}

std::vector<C8Proc> C8StateLog::getLog(void) const
{
    return this->log;
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

std::string C8Exec::toString(void)
{
    std::ostringstream oss;

    // Top line 
    oss << " u  p  Vx  Vy kk  nnn " << std::endl;
    oss << " " << std::hex << std::setw(2) << std::setfill('0') << this->u;
    oss << " " << std::hex << std::setw(2) << std::setfill('0') << this->p;
    oss << " " << std::hex << std::setw(2) << std::setfill('0') << this->vx;
    oss << " " << std::hex << std::setw(2) << std::setfill('0') << this->vy;
    oss << " " << std::hex << std::setw(2) << std::setfill('0') << this->kk;
    oss << " " << std::hex << std::setw(2) << std::setfill('0') << this->nnn;

    return oss.str();
}

/*
 * Chip8
 * Chip-8 object
 */
Chip8::Chip8()
{
    this->mem_size = 0x1000;
    this->init_mem();
    this->log_exec = false;
    this->log_state = false;
    this->state.init();
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
 * exec_zero_op()
 */
void Chip8::exec_zero_op(void)
{
    uint16_t zero_code = this->cur_opcode & 0x0FFF;
    switch(zero_code)
    {
        case 0x00E0:        // CLS 
            std::cout << "[" << __func__ << "] got CLS" << std::endl;
            break;

        case 0x00EE:        // RET 
            this->state.sp--;
            this->state.pc = this->state.stack[this->state.sp % 12];
            break;

        default:            // SYS  (nop in this interpreter)
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] got SYS (0x" <<
                   std::hex << std::setw(2) << std::setfill('0') << 
                   unsigned(this->cur_opcode) << ")" << std::endl;
            }
            break;
    }
}

/*
 * cycle()
 * Run through the instruction cycle 
 */
void Chip8::cycle(void)
{
    uint16_t instr_code;
    uint8_t  r;              // result
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

    // Modify the code here for the jump table 
    switch(this->exec.u)
    {
        case 0x0:
            instr_code = 0x0000;
            break;

        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
            instr_code &= 0xFFF0;
            break;

        default:
            break;
    }

    //std::cout << "[" << __func__ << "] instr_code " 
    //    << std::hex << std::setw(4) << std::setfill('0')
    //    << instr_code << std::endl;

    switch(instr_code)
    {
        case C8_ZERO:
            this->exec_zero_op();
            break;

        case C8_JP:     // JP nnn
            this->state.pc = this->exec.nnn;
            break;

        case C8_CALL:   // CALL nnn
            this->state.stack[this->state.sp % 12] = this->state.pc;
            this->state.sp++;
            this->state.pc = this->exec.nnn;
            break;

        case C8_SEI:    // SE Vx, kk
            if(this->state.V[this->exec.vx] == this->exec.kk)
                this->state.pc += 2;
            break;

        case C8_SNEI:   // SNE Vx, kk
            if(this->state.V[this->exec.vx] != this->exec.kk)
                this->state.pc += 2;
            break;

        case C8_SE:     // SE Vx Vy
            if(this->state.V[this->exec.vx] == this->state.V[this->exec.vy])
                this->state.pc += 2;
            break;

        case C8_LDI:    // LD Vx, kk
            this->state.V[this->exec.vx] = this->exec.kk;
            break;

        case C8_ADDI:   // ADD Vx, kk
            this->state.V[this->exec.vx] += this->exec.kk;
            break;

        case C8_LD:     // LD Vx, Vy
            this->state.V[this->exec.vx] = this->state.V[this->exec.vy];
            break;

            // Arithmetic section
        case C8_OR:     // OR Vx, Vy
            this->state.V[this->exec.vx] |= this->state.V[this->exec.vy];
            break;

        case C8_AND:    // AND Vx, Vy
            this->state.V[this->exec.vx] &= this->state.V[this->exec.vy];
            break;

        case C8_XOR:    // XOR Vx, Vy
            this->state.V[this->exec.vx] ^= this->state.V[this->exec.vy];
            break;

        case C8_ADD:    // ADD Vx, Vy
            r = this->state.V[this->exec.vx] + this->state.V[this->exec.vy];
            this->state.V[0xf] = (r >> 8);
            this->state.V[this->exec.vx] = r;
            break;

        case C8_SUB:    // SUB Vx, Vy
            r = this->state.V[this->exec.vx] - this->state.V[this->exec.vy];
            this->state.V[0xf] = !(r >> 8);
            this->state.V[this->exec.vx] = r;
            break;

        case C8_SHR:    // SHR Vx
            this->state.V[0xF] = this->state.V[this->exec.vy] & 0x1;
            this->state.V[this->exec.vx] = this->state.V[this->exec.vy] >> 1;
            break;

        case C8_SUBN:    // SUBN Vx, Vy
            r = this->state.V[this->exec.vy] - this->state.V[this->exec.vx];
            this->state.V[0xf] = !(r >> 8);
            this->state.V[this->exec.vx] = r;
            break;

        case C8_SHL:    // SHL, Vx
            this->state.V[0xF] = this->state.V[this->exec.vy] >> 7;
            this->state.V[this->exec.vx] = this->state.V[this->exec.vy] << 1;
            break;

        case C8_SNE:    // SNE Vx, Vy
            if(this->state.V[this->exec.vx] != this->state.V[this->exec.vy])
                this->state.pc += 2;
            break;

        case C8_LDII:   // LD I, nnn
            this->state.I = this->exec.nnn;
            break;

        case C8_JPV:    // JP V0, nnn
            this->state.pc = this->state.V[0x0] + this->exec.nnn;
            break;

        case C8_RND:
            this->state.V[this->exec.vx] = std::uniform_int_distribution<>(0,255)(rnd) & this->exec.kk;
            break;

        case C8_DRW:
            std::cout << "[" << __func__ << "] TODO: DRW" << std::endl;
            break;

        case C8_SKP:    // SKP Vx
            if(this->state.keys[this->state.V[this->exec.vx] & 0xF])
               this->state.pc += 2; 
            break;

        case C8_SKNP:   // SNKP Vx
            if(!this->state.keys[this->state.V[this->exec.vx] & 0xF])
               this->state.pc += 2; 
            break;

        case C8_LDB:    // LD B, Vx
            this->mem[this->state.I & 0xFFF] = (this->state.V[this->exec.vx] / 100) % 10;
            this->mem[this->state.I & 0xFFF] = (this->state.V[this->exec.vx] / 100) % 10;
            this->mem[this->state.I & 0xFFF] = (this->state.V[this->exec.vx] / 100) % 10;
            break;

        case C8_LDIVX:  // LD [I], Vx
            for(r = 0; r < this->exec.vx; ++r)
            {
                this->mem[this->state.I & 0xFFF] = this->state.V[r];
                this->state.I++;
            }
            break;

        case C8_LDVXI:  // LD Vx, [I]
            for(r = 0; r < this->exec.vx; ++r)
            {
                this->state.V[r] = this->mem[this->state.I & 0xFFF];
                this->state.I++;
            }
            break;

        default:
            std::cout << "[" << __func__ << "] invalid op <0x" 
                << std::hex << std::setw(4) << std::setfill('0') 
                << this->cur_opcode << ">" << "instr code <" 
                << std::hex << std::setw(4) << std::setfill('0') 
                << instr_code << ">" << std::endl;
            break;
    }

    if(this->log_state)
        this->state_log.add(this->state);
}

/*
 * loadMem()
 */
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
        std::cerr << "[" << __func__ << "] caught execption [%s]" << 
            e.what() << " while reading file " << filename << std::endl;
        status = -1;
    }

    return status;
}

/*
 * loadMem()
 */
void Chip8::loadMem(const std::vector<uint8_t>& data, int offset)
{
    unsigned int addr = offset;
    for(unsigned int i = 0; i < data.size(); ++i)
    {
        this->mem[addr] = data[i];
        addr++;
    }
}

/*
 * dumpMem()
 */
std::vector<uint8_t> Chip8::dumpMem(void) const
{
    std::vector<uint8_t> mem_out(this->mem_size);
    for(unsigned int m = 0; m < this->mem_size; ++m)
        mem_out[m] = this->mem[m];
        //mem_out.push_back(this->mem[m]);

    return mem_out;
}

/* 
 * readMem()
 */
uint8_t Chip8::readMem(const unsigned int addr) const
{
    return this->mem[addr];
}

void Chip8::setTrace(const bool v)
{
    this->log_state = v;
}

std::vector<C8Proc> Chip8::getTrace(void)
{
    return this->state_log.getLog();
}

C8Proc Chip8::getState(void) const
{
    return this->state;
}

void Chip8::setVerbose(const bool v)
{
    this->verbose = v;
}
bool Chip8::getVerbose(void) const
{
    return this->verbose;
}
