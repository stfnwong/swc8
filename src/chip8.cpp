/* CHIP-8
 * A Chip-8 object for emulating the Chip-8 VM
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstring>      //for memset()
#include "chip8.hpp"

/*
 * C8Proc
 * Chip-8 processor state
 */
C8Proc::C8Proc()
{
    this->init();
}

/*
 * C8Proc()
 */
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

/*
 * init()
 */
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
    this->keypress = 0;
}


/*
 * toString()
 */
std::string C8Proc::toString(void) const
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    // Program counter, Stack pointer, I register
    oss << " [PC]     [SP]    [I]     DT    ST" << std::endl;
    oss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << this->pc << "  ";
    oss << " 0x" << std::hex << std::setw(2) << std::setfill('0') << this->sp << "   ";
    oss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << this->I << " ";
    oss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << this->dt << " ";
    oss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << this->st << " ";

    oss << std::endl;

    // Try to cram 8 registers on each line
    // First 8
    for(int v = 0; v < 8; ++v)
        oss << "  V" << std::hex << std::setw(1) << unsigned(v) << "  ";
    oss << std::endl;

    for(int v = 0; v < 8; ++v)
    {
        oss << "[0x" << std::hex << std::setw(2) << std::uppercase
            << std::setfill('0') << unsigned(this->V[v]) << "]";
    } 
    oss << std::endl;
    // Next 8
    for(int v = 8; v < 16; ++v)
        oss << "  V" << std::hex << std::setw(1) << unsigned(v) << "  ";
    oss << std::endl;
    for(int v = 8; v < 16; ++v)
    {
        oss << "[0x" << std::hex << std::setw(2) << std::uppercase
            << std::setfill('0') << unsigned(this->V[v]) << "]";
    } 

    oss << std::endl;

    // Add stack - this might fit on one like
    for(int s = 0; s < 12; ++s)
        oss << "  S" << std::hex << std::setw(1) << unsigned(s) << " ";
    oss << std::endl;
    for(int s = 0; s < 12; ++s)
    {
        oss << "0x" << std::hex << std::setw(2) << std::setfill('0') 
            << std::uppercase << unsigned(this->stack[s]) << " ";
    }
    oss << std::endl;

    oss << "---------------------------------------------------------------------" << std::endl;

    return oss.str();
}


/*
 * diffStr()
 */
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
    if(this->dt != that.dt)
    {
        oss << "this->dt (" << std::hex << std::setw(4) 
            << std::setfill('0') << unsigned(this->dt) << ") != that.dt ("
            << std::setw(4) << std::setfill('0') << unsigned(that.dt)
            << ")" << std::endl;
    }
    if(this->st != that.st)
    {
        oss << "this->st (" << std::hex << std::setw(4) 
            << std::setfill('0') << unsigned(this->st) << ") != that.st ("
            << std::setw(4) << std::setfill('0') << unsigned(that.st)
            << ")" << std::endl;
    }

    return oss.str();
}

/*
 * ==
 */
bool C8Proc::operator==(const C8Proc& that) const
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

/*
 * !=
 */
bool C8Proc::operator!=(const C8Proc& that) const
{
    return (*this == that);
}


/*
 * C8Exec
 * Chip-8 execution context. Allows saving and resuming
 * of pipeline state.
 */
C8Exec::C8Exec() 
{
    this->init();
}


bool C8Exec::operator==(const C8Exec& that) const
{
    if(this->u != that.u)
        return false;
    if(this->p != that.p)
        return false;
    if(this->vx != that.vx)
        return false;
    if(this->vy != that.vy)
        return false;
    if(this->kk != that.kk)
        return false;
    if(this->nnn != that.nnn)
        return false;

    return true;
}

bool C8Exec::operator!=(const C8Exec& that) const
{
    return !(*this == that);
}

/*
 * C8Exec::toString()
 */
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
 * C8Exec::init()
 */
void C8Exec::init(void)
{
    this->u   = 0;
    this->p   = 0;
    this->vx  = 0;
    this->vy  = 0;
    this->kk  = 0;
    this->nnn = 0;
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

/*
 * copy ctor
 */
//Chip8::Chip8(const Chip8& that)
//{
//    this->state    = that.state;
//    this->mem_size = that.mem_size;
//    for(int m = 0; m < this->mem_size; m++)
//        this->mem[m] = that.mem[m];
//}

/*
 * init_mem()
 */
void Chip8::init_mem(void)
{
    for(unsigned int m = 0; m < this->mem_size; ++m)
        this->mem[m] = 0;
    // insert JP 0x200 instruction at start of memory
    this->mem[0] = 0x12;
    this->mem[1] = 0x00;

    // init font mem
    int n = 16;          // pick an address where font mem starts
    for(auto f : FONT_MEM)
    {
        this->mem[n] = (f & 0xFF);
        n++;
    }
}

/*
 * exec_zero_op()
 */
void Chip8::exec_zero_op(void)
{
    uint16_t zero_code = this->cur_opcode & 0x0FFF;
    switch(zero_code)
    {
        case 0x00E0:        // CLS  (not yet implemented)
            memset(this->disp_mem, 0, DISP_MEM_SIZE);
            // TODO : set draw flag
            break;

        // TODO: trap overflow/underflow
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
 * Chip8::draw()
 */
void Chip8::draw(void)
{
    //uint16_t sprite_ptr = this->state.I;
    uint8_t flip = 0;
    int xpos, ypos;
    int xline, yline;
    uint8_t cur_bit, cur_sprite;
    uint8_t cur_pixel;

    // sprite pos
    xpos = this->exec.vx % DISP_W;
    ypos = this->exec.vy % DISP_H;
    this->state.V[0xF] = 0;

    for(yline = 0; yline < this->exec.nnn; ++yline)
    {
        for(xline = 0; xline < 8; ++xline)
        {
            this->disp_mem[(ypos + yline) * xpos + xline] = 1;
        }
    }

    //for(yline = 0; yline < this->exec.nnn; ++yline)
    //{
    //    cur_pixel = this->disp_mem[this->state.I + yline];
    //    for(xline = 0; xline < 8; ++xline)
    //    {
    //        cur_bit = cur_pixel & (0x80 >> xline);
    //        if(cur_bit != 0)
    //        {
    //            // wrap or clip (depends on game)
    //            if((xpos + xline) > DISP_W)
    //                break;
    //                //continue;

    //            if(this->disp_mem[(ypos + yline) * xpos + xline] == 0x1)
    //            {
    //                this->state.V[0xF] = 1;
    //            }
    //            this->disp_mem[(ypos + yline) * xpos + xline] ^= 0x1;
    //        }

    //        //if(cur_pixel & (0x80 >> xline) != 0)
    //        //{
    //        //    this->disp_mem[xpos + xline + ((ypos + yline) * DISP_W)] ^= 1;
    //        //}
    //        //if(this->disp_mem[xpos + xline + ((ypos + yline) * DISP_W)] == 1)
    //        //    this->state.V[0xF] = 1;

    //        //uint8_t disp_x = (xpos + xline);// % DISP_W;
    //        //uint8_t disp_y = (ypos + yline);// % DISP_H;
    //        //this->disp_mem[disp_y * DISP_H + disp_x] = 1;
    //        //this->disp_mem[(disp_x * DISP_W) + disp_y] = 1; // this should fill the whole pixel area
    //        //this->disp_mem[(disp_y * DISP_H) + disp_x] = 1; // this should fill the whole pixel area
    //    }
    //}
}


void Chip8::debug_draw(void)
{
    int x, y;

    for(y = 0; y < DISP_W; ++y)
    {
        for(x = 0; x < DISP_W; ++x)
        {
            if((x % 2 == 0) && (y % 2 == 0))
                this->disp_mem[y * DISP_H + x] = 1;
            else
                this->disp_mem[y * DISP_H + x] = 0;
        }
    }
}


void Chip8::keypress(void)
{

}

/*
 * Chip8::cycle()
 * Run through the instruction cycle 
 */
void Chip8::cycle(void)
{
    uint16_t instr_code;
    uint8_t  r;              // result
    // decode 
    this->cur_opcode = (this->mem[this->state.pc] << 8) |  
                       (this->mem[this->state.pc + 1]);

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

    // increment program counter
    this->state.pc += 2;

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
            // need to wind back the program counter
            //this->state.pc -= 2;
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

        case C8_DRW:    // D Vx, Vy, N
            this->debug_draw();
            //this->draw();
            break;

        case C8_SKP:    // SKP Vx
            if(this->state.keys[this->state.V[this->exec.vx] & 0xF])
               this->state.pc += 2; 
            break;

        case C8_SKNP:   // SNKP Vx
            if(!this->state.keys[this->state.V[this->exec.vx] & 0xF])
               this->state.pc += 2; 
            break;

        case C8_LDK:    // LD Vx, K
            // algorithm is something like 
            // if(no key waiting)
            // {
            //   key_waiting = true;
            //   save_key_flags = key_flags;
            //   stay on KEY instr without advancing PC
            // }
            // else
            // {
            //   if(key pressed)
            //   {
            //     move key into Vx
            //     key_waiting = false;
            //     advance PC and return;
            //   }
            //   else
            //     stay on KEY instr, don't advance PC, return
            std::cout << "[" << __func__ << "] LD Vx K not yet implemented." << std::endl;
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
                << this->cur_opcode << ">" << " instr code <" 
                << std::hex << std::setw(4) << std::setfill('0') 
                << instr_code << ">" << std::endl;
            break;
    }

    if(this->log_state)
        this->state_log.add(this->state);
}


/*
 * Chip8::renderTo()
 */
// TODO : debug mode : should fill the whole screen
void Chip8::renderTo(uint32_t* pixels, int W, int H)
{
    for(int pos = 0; pos < (W * H); ++pos)
        pixels[pos] = pos << 8;
        //pixels[pos] = 0xFFFFFFFF * this->disp_mem[pos];
        //pixels[pos] = 0xFFFFFF * ((this->disp_mem[pos / 8] >> (7 - pos % 8)) & 1);
        //pixels[pos] = 0xF;
        //pixels[pos] = 0xFFFFFFFF;
}

/*
 * Chip8::getKey()
 */
uint8_t Chip8::getKey(uint8_t k) const
{
    if(k < 16)
        return this->state.keys[k];

    return 0;
}

/*
 * Chip8::getKeyPress()
 */
uint8_t Chip8::getKeyPress(void) const
{
    return (this->state.keypress & 0x80);
}

/*
 * Chip8::updateKey()
 */
void Chip8::updateKey(int k, uint8_t v)
{
    this->state.keys[k] = v;
}

/*
 * Chip8::setKeyPress()
 */
void Chip8::setKeyPress(uint8_t kp)
{
    this->state.keypress &= 0x7F;
    this->state.keys[this->state.keypress] = kp;
}

/*
 * Chip8::getST()
 */
uint8_t Chip8::getST(void) const
{
    return this->state.st;
}

/*
 * Chip8::getDT()
 */
uint8_t Chip8::getDT(void) const
{
    return this->state.dt;
}

/*
 * Chip8::setST()
 */
void Chip8::setST(uint8_t s)
{
    this->state.st = s;
}

/*
 * Chip8::setDT()
 */
void Chip8::setDT(uint8_t d)
{
    this->state.dt = d;
}

/*
 * Chip8::loadMem()
 */
int Chip8::loadMem(const std::string& filename, int offset=0x200)
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
 * Chip8::loadMem()
 */
void Chip8::loadMem(const std::vector<uint8_t>& data, int offset=0x200)
{
    unsigned int addr = offset;
    for(unsigned int i = 0; i < data.size(); ++i)
    {
        this->mem[addr] = data[i];
        addr++;
    }
}

/*
 * Chip8::dumpMem()
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
 * Chip8::readMem()
 */
uint8_t Chip8::readMem(const unsigned int addr) const
{
    return this->mem[addr];
}

unsigned int Chip8::getMemSize(void) const
{
    return C8_MEM_SIZE;
}

const uint8_t* Chip8::getMemArray(void) const
{
    return this->mem;
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

