/* PROGRAM
 * ROutines for manipulating a Chip-8 program
 *
 * Stefan Wong 2018
 */

#include <sstream>
#include <iomanip>
#include "program.hpp"

void initInstr(Instr& i)
{
    i.adr = 0;
    i.ins = 0;
}

Program::Program() {} 

Program::~Program() {} 

Program::Program(const Program& that)
{
    this->verbose = that.verbose;
    for(unsigned int i = 0; i < that.instructions.size(); ++i)
        this->instructions.push_back(that.instructions[i]);
}

/*
 * instr_to_string()
 */
std::string Program::instr_to_string(const Instr& i)
{
    std::ostringstream oss;

    oss << "[" << std::hex << std::setw(4) << std::setfill('0') << i.adr << "]  ";
    oss << " $" << std::hex << std::setw(4) << std::setfill('0') << std::left << i.ins << std::endl;

    return oss.str();
}

/*
 * initProgram()
 */
void Program::initProgram(void)
{
    this->instructions.erase(this->instructions.begin(), this->instructions.end());
}

/*
 * get()
 */
Instr Program::get(const unsigned int idx) const
{
    if(idx > 0 && idx < this->instructions.size())
        return this->instructions[idx];

    Instr i;
    initInstr(i);
    return i;
}

/*
 * getStr()
 */
std::string Program::getStr(const unsigned int idx)
{
    return this->instr_to_string(this->instructions[idx]);
}

void Program::add(const Instr& i)
{
    this->instructions.push_back(i);
}

void Program::writeMem(const uint16_t addr, const uint16_t val)
{
    Instr ins;

    ins.adr = addr;
    ins.ins = val;
    this->instructions.push_back(ins);
}

unsigned int Program::numInstr(void) const
{
    return this->instructions.size();
}


void Program::setVerbose(const bool v)
{
    this->verbose = v;
}
bool Program::getVerbose(void) const
{
    return this->verbose;
}
