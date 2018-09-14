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
inline uint8_t Disassembler::dis_n(const uint16_t instr)
{
    return (instr & 0x000F);
}
inline uint16_t Disassembler::dis_nnn(const uint16_t instr)
{
    return (instr & 0x0FFF);
}


// TODO : lexer independant symbols

void Disassembler::dis_ldvxvy(void)
{
    this->cur_line.opcode.opcode = LEX_LD;  
    this->cur_line.opcode.mnemonic = "LD";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.vy = this->dis_vy(this->cur_instr);
}

void Disassembler::dis_ldkk(void)
{
    this->cur_line.opcode.opcode = LEX_LD;  
    this->cur_line.opcode.mnemonic = "LD";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.kk = this->dis_kk(this->cur_instr);
    this->cur_line.is_imm = true;
}

void Disassembler::dis_ldi(void)
{
    this->cur_line.opcode.opcode = LEX_LD;  
    this->cur_line.opcode.mnemonic = "LD";
    this->cur_line.nnn = this->dis_nnn(this->cur_instr);
    this->cur_line.is_imm = true;
    this->cur_line.reg_flags = LEX_IREG;
}


void Disassembler::dis_sevxvy(void)
{
    this->cur_line.opcode.opcode = LEX_SE;  
    this->cur_line.opcode.mnemonic = "SE";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.vy = this->dis_vy(this->cur_instr);
}

void Disassembler::dis_sekk(void)
{
    this->cur_line.opcode.opcode = LEX_SE;  
    this->cur_line.opcode.mnemonic = "SE";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.kk = this->dis_kk(this->cur_instr);
    this->cur_line.is_imm = true;
}

void Disassembler::dis_snevxvy(void)
{
    this->cur_line.opcode.opcode = LEX_SE;
    this->cur_line.opcode.mnemonic = "SE";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.vy = this->dis_vy(this->cur_instr);
}

void Disassembler::dis_snekk(void)
{
    this->cur_line.opcode.opcode = LEX_SE;
    this->cur_line.opcode.mnemonic = "SE";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.kk = this->dis_kk(this->cur_instr);
    this->cur_line.is_imm = true;
}




/*
 * dis_arith()
 */
void Disassembler::dis_arith(void)
{
    uint8_t arith_mask;

    arith_mask = this->cur_instr & 0x000F;
    switch(arith_mask)
    {
        case 0x0:   //LD Vx Vy 
            this->dis_ldvxvy();
            break;

        case 0x1:
            this->cur_line.opcode.opcode = LEX_OR;
            this->cur_line.opcode.mnemonic = "OR";
            break;

        case 0x2:
            this->cur_line.opcode.opcode = LEX_AND;
            this->cur_line.opcode.mnemonic = "AND";
            break;

        case 0x03:
            this->cur_line.opcode.opcode = LEX_XOR;
            this->cur_line.opcode.mnemonic = "XOR";
            break;

        case 0x4:
            this->cur_line.opcode.opcode = LEX_ADD;
            this->cur_line.opcode.mnemonic = "ADD";
            break;

        case 0x5:
            this->cur_line.opcode.opcode = LEX_SUB;
            this->cur_line.opcode.mnemonic = "SUB";
            break;

        case 0x6:
            this->cur_line.opcode.opcode = LEX_SHR;
            this->cur_line.opcode.mnemonic = "SHR";
            break;

        case 0x7:
            this->cur_line.opcode.opcode = LEX_SUBN;
            this->cur_line.opcode.mnemonic = "SUBN";
            break;

        default:
            std::cerr << "[" << __FUNCTION__ << "] unknown arithmetic op in instruction <" << std::hex << std::setw(4) << this->cur_instr << ">" << std::endl;
            break;
    }
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.vy = this->dis_vy(this->cur_instr);
}



void Disassembler::dis_skp(void)
{
    uint8_t skp_mask;

    skp_mask = this->cur_instr & 0x000F;
    switch(skp_mask)
    {
        case 0x1:
            this->cur_line.opcode.opcode = LEX_SKNP;
            this->cur_line.opcode.mnemonic = "SKNP";
            break;

        case 0xE:
            this->cur_line.opcode.opcode = LEX_SKP;
            this->cur_line.opcode.mnemonic = "SKP";
            break;

        default:
            this->cur_line.error = true;
            this->cur_line.errstr = "unknown SKP opcode " + std::to_string(this->cur_instr);
            if(this->verbose)
                std::cout << "[" << __FUNCTION__ << "] " << this->cur_line.errstr << std::endl;
    }

    this->cur_line.vx = this->dis_vx(this->cur_instr);
}

void Disassembler::dis_ld_special(void)
{
    uint8_t ld_mask;

    ld_mask = (this->cur_instr & 0x00FF);
    this->cur_line.opcode.opcode = LEX_LD;
    this->cur_line.opcode.mnemonic = "LD";
    switch(ld_mask)
    {
        case 0x07:      // LD Vx, DT
            this->cur_line.vx = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_DTREG;
            break;

        case 0x0A:      // LD Vx, K
            this->cur_line.vx = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_KREG;

        case 0x15:      // LD DT, Vx
            this->cur_line.vy = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_DTREG;
            break;

        case 0x18:      // LD ST, Vx
            this->cur_line.vy = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_STREG;

        case 0x1E:      // ADD I, Vx
            break;

        case 0x29:      // LD F, Vx
            this->cur_line.vy = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_FREG;
            break;

        case 0x33:      // LD B, Vx
            this->cur_line.vy = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_BREG;
            break;

        case 0x55:      // LD [I], Vx
            this->cur_line.vy = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_IST;
            break;

        case 0x65:      // LD Vx, [I]
            this->cur_line.vx = this->dis_vx(this->cur_instr);
            this->cur_line.reg_flags = LEX_ILD;
            break;


        default:
            this->cur_line.error = true;
            this->cur_line.errstr = "Unknown LD opcode " + std::to_string(this->cur_instr);
            if(this->verbose)
                std::cout << "[" << __FUNCTION__ << "] " << this->cur_line.errstr << std::endl;
    }
}

void Disassembler::dis_rnd(void)
{
    this->cur_line.opcode.opcode = LEX_RND;
    this->cur_line.opcode.mnemonic = "RND";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.kk = this->dis_kk(this->cur_instr);
}

void Disassembler::dis_drw(void)
{
    this->cur_line.opcode.opcode = LEX_DRW;
    this->cur_line.opcode.mnemonic = "DRW";
    this->cur_line.vx = this->dis_vx(this->cur_instr);
    this->cur_line.vy = this->dis_vy(this->cur_instr);
    this->cur_line.kk = this->dis_n(this->cur_instr);
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
        instr_mask = (instr.ins & 0xF000) >> 12;
        this->cur_instr = instr.ins;
        initLineInfo(this->cur_line);
        if(this->verbose)
        {
            std::cout << "[" << __FUNCTION__ << "] testing intr_mask " 
                << std::hex << std::setw(2) << instr_mask << std::endl;
        }
        switch(instr_mask)
        {
            case 0x0:       // TODO : named constants 
                break;

            case 0x3:
                this->dis_sekk();
                break;

            case 0x4:
                this->dis_snekk();
                break;

            case 0x5:
                this->dis_sevxvy();
                break;

            case 0x6:
                this->dis_ldkk();
                break;

            case 0x8:
                this->dis_arith();
                break;

            case 0xA: // LD I nnn
                this->dis_ldi();
                break;

            case 0xC:   // RND Vx, kk
                this->dis_rnd();
                break;

            case 0xD:   // DRW Vx. Vy, n
                this->dis_drw();
                break;

            case 0xE:
                this->dis_skp();
                break;

            case 0xF:       // special registers
                this->dis_ld_special();
                break;

            default:
                this->cur_line.error = true;
                this->cur_line.errstr = "Unknown instruction opcode";
                std::cerr << "[" << __FUNCTION__ << "] unknown instrucion opcode 0x" << std::hex << std::setw(4) << instr.ins << std::endl;
                break;;
        }

        this->cur_line.line_num = i+1;
        this->cur_line.addr = instr.adr;
        this->source.add(this->cur_line);
    }
}


SourceInfo Disassembler::getSourceInfo(void) const
{
    return this->source;
}
Program Disassembler::getProgram(void) const
{
    return this->program;
}
void Disassembler::setVerbose(const bool v)
{
    this->verbose = v;
}
bool Disassembler::getVerbose(void) const
{
    return this->verbose;
}
