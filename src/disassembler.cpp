/*
 * Disassembler
 * Chip-8 disassembler
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include "disassembler.hpp"
#include "lexer.hpp"

Disassembler::Disassembler()
{
    this->verbose = false;
}

Disassembler::~Disassembler() {} 

/*
 * Instruction disassembly
 */
inline uint8_t Disassembler::dis_vx(const uint16_t instr)
{
    return (instr & 0x0F00) >> 8;
}
inline uint8_t Disassembler::dis_vy(const uint16_t instr)
{
    return (instr & 0x00F0) >> 4;
}
inline uint8_t Disassembler::dis_kk(const uint16_t instr)
{
    return (instr & 0x00FF);
}
inline uint16_t Disassembler::dis_nnn(const uint16_t instr)
{
    return (instr & 0x0FFF);
}


void Disassembler::dis_sevxvy(const uint16_t instr)
{
    // TODO : lexer independant symbols
    this->cur_line.opcode.opcode = LEX_SE;  
    this->cur_line.opcode.mnemonic = "SE";
    this->cur_line.vx = this->dis_vx(instr);
    this->cur_line.vy = this->dis_vy(instr);
}

void Disassembler::dis_sekk(const uint16_t instr)
{
    this->cur_line.opcode.opcode = LEX_SE;  
    this->cur_line.opcode.mnemonic = "SE";
    this->cur_line.vx = this->dis_vx(instr);
    this->cur_line.kk = this->dis_kk(instr);
}

/*
 * dis_arith()
 */
void Disassembler::dis_arith(const uint16_t instr)
{
    uint8_t arith_mask;

    arith_mask = instr & 0x000F;
    switch(arith_mask)
    {
        default:
            std::cerr << "[" << __FUNCTION__ << "] unknown arithmetic op in instruction <" << std::hex << std::setw(4) << instr << ">" << std::endl;
            break;
    }
}


/*
 * load()
 */
int Disassembler::load(const std::string& filename)
{
    return this->program.load(filename);
}

/*
 * disassemble()
 */
void Disassembler::disassemble(void)
{
    unsigned int i;
    uint16_t instr_mask;
    Instr instr;

    if(this->program.numInstr() == 0)
    {
        std::cerr << "[" << __FUNCTION__ << "] No instructions in program" << std::endl;
        return;
    }

    // Disassemble each line in turn
    for(i = 0; i < this->program.numInstr(); ++i)
    {
        instr = this->program.get(i);
        instr_mask = instr.ins & 0xF000;
        switch(instr_mask)
        {
            case 0x0:       // TODO : named constants 
                break;

            case 0x4:
                break;

            default:
                std::cerr << "[" << __FUNCTION__ << "] unknown instrucion opcode 0x" << std::hex << std::setw(4) << instr.ins << std::endl;
                return;
        }

        this->cur_line.line_num = i;
        this->source.add(this->cur_line);
    }
}


SourceInfo Disassembler::getSourceInfo(void) const
{
    return this->source;
}

void Disassembler::setVerbose(const bool v)
{
    this->verbose = v;
}
bool Disassembler::getVerbose(void) const
{
    return this->verbose;
}
