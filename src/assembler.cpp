/* ASSEMBLER 
 * Chip8 psuedo-assembler 
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include "assembler.hpp"
#include "lexer.hpp"

Assembler::Assembler()
{
    this->verbose = false;
}

Assembler::~Assembler() {}

Assembler::Assembler(const Assembler& that)
{
    this->source = that.source;
    this->program = that.program;
}

// ==== Assembly routines 

inline uint8_t Assembler::get_reg(const uint16_t r)
{
    return (r & 0x000F);
}
inline uint16_t Assembler::insert_vxvy(const uint16_t vx, const uint16_t vy)
{
    uint16_t ins = 0x0000;
    ins = ins | ((vx & 0x00FF) << 8);
    ins = ins | ((vy & 0x00FF) << 4);

    return ins;
}
inline uint16_t Assembler::insert_vxkk(const uint16_t vx, const uint16_t kk)
{
    uint16_t ins = 0x0000;
    ins = ins | (vx << 8);
    ins = ins | (kk & 0x00FF);
    return ins;
}
inline uint16_t Assembler::insert_addr(const uint16_t addr)
{
    return 0x0000 | (addr & 0x0FFF);
}

/*
 * asm_add()
 */
void Assembler::asm_add(const LineInfo& l)
{
    Instr instr;

    if(l.is_imm)
        instr.ins = 0x7000 | this->insert_vxkk(l.vx, l.kk);
    else
        instr.ins = 0x8004 | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_and()
 */
void Assembler::asm_and(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x8002;
    instr.ins = instr.ins | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_drw()
 */
void Assembler::asm_drw(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0xD000 | this->insert_vxvy(l.vx, l.vy);
    instr.ins = instr.ins | l.kk;
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_jp()
 */
void Assembler::asm_jp(const LineInfo& l)
{
    Instr instr;

    //if(l.vx == 0)
    //    instr.ins = 0xB000 | this->insert_addr(l.nnn);
    //else
    //    instr.ins = 0x1000 | this->insert_addr(l.nnn);
    instr.ins = 0x1000 | this->insert_addr(l.nnn);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_ld()
 */
void Assembler::asm_ld(const LineInfo& l)
{
    Instr instr;

    if(l.ireg)
    {
        std::cout << "nnn = " << std::hex << l.nnn << std::endl;
        instr.ins = 0xA000;
        instr.ins = instr.ins | (l.nnn & 0x0FFF);
    }
    else if(l.is_imm)
    {
        instr.ins = 0x6000;
        instr.ins = instr.ins | this->insert_vxkk(l.vx, l.kk);
    }
    else
    {
        instr.ins = 0x8000;
        instr.ins = instr.ins | this->insert_vxvy(l.vx, l.vy);
    }
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_or()
 */
void Assembler::asm_or(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x8001 | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_rnd()
 */
void Assembler::asm_rnd(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0xC000 | this->insert_vxkk(l.vx, l.kk);
    instr.adr = l.addr;
    this->program.add(instr);
}

/* 
 * asm_se()
 */
void Assembler::asm_se(const LineInfo& l)
{
    Instr instr;

    if(l.is_imm)
    {
        instr.ins = 0x3000;
        instr.ins = instr.ins | this->insert_vxkk(l.vx, l.kk);
    }
    else
    {
        instr.ins = 0x5000;
        instr.ins = instr.ins | this->insert_vxvy(l.vx, l.vy);
    }
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_shl()
 */
void Assembler::asm_shl(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x800E | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_shr()
 */
void Assembler::asm_shr(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x8006 | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_sne()
 */
void Assembler::asm_sne(const LineInfo& l)
{
    Instr instr;

    if(l.is_imm)
        instr.ins = 0x4000 | this->insert_vxkk(l.vx, l.kk);
    else
        instr.ins = 0x9000 | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

/*
 * asm_sub()
 */
void Assembler::asm_sub(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x8005 | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_subn(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x8007 | this->insert_vxvy(l.vx, l.vy);
    instr.adr = l.addr;
    this->program.add(instr);
}


/*
 * assemble()
 */
void Assembler::assemble(void)
{
    LineInfo cur_line;

    this->program.initProgram();
    if(this->source.getNumLines() == 0)
    {
        std::cout << "[" << __FUNCTION__ << "] error - no lines in source" << std::endl;
        return;
    }

    for(unsigned int i = 0; i < this->source.getNumLines(); ++i)
    {
        // TODO: why do we need special code here? Should they be factored
        // out of the lexer and into some common source file?
        cur_line = this->source.get(i);
        switch(cur_line.opcode.opcode)
        {
            case LEX_ADD:
                this->asm_add(cur_line);
                break;

            case LEX_AND:
                this->asm_and(cur_line);
                break;

            case LEX_DRW:
                this->asm_drw(cur_line);
                break;

            case LEX_JP:
                this->asm_jp(cur_line);
                break;

            case LEX_LD:
                this->asm_ld(cur_line);
                break;

            case LEX_SE:
                this->asm_se(cur_line);
                break;

            default:
                if(this->verbose)
                {
                    std::cout << "[" << __FUNCTION__ << "] (line " 
                        << std::dec << cur_line.line_num 
                        << ") Unknown lexer code " << std::hex 
                        << cur_line.opcode.opcode << std::endl;
                }

                break;
        }
    }
}

void Assembler::loadSource(const SourceInfo& s)
{
    this->source = s;
}

Program Assembler::getProgram(void) const
{
    std::cout << "[" << __FUNCTION__ << "] program has " << std::dec 
        << this->program.numInstr() << " instructions " << std::endl;
    return this->program;
}

void Assembler::setVerbose(const bool v)
{
    this->verbose = v;
}
bool Assembler::getVerbose(void) const
{
    return this->verbose;
}

