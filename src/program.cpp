/* PROGRAM
 * ROutines for manipulating a Chip-8 program
 *
 * Stefan Wong 2018
 */

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
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

/*
 * save()
 */
int Program::save(const std::string& filename)
{
    uint16_t N;
    std::ofstream outfile;

    try {
        outfile.open(filename, std::ios::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __FUNCTION__ << "] " << e.what() << std::endl;
        return -1;
    }

    N = (uint16_t) this->instructions.size();
    outfile.write(reinterpret_cast<char*>(&N), sizeof(uint16_t));

    // Debug, remove 
    std::cout << "[" << __FUNCTION__ << "] first address is " 
        << std::hex << std::setw(4) << this->instructions[0].adr 
        << std::endl;

    outfile.write(reinterpret_cast<char*>
                (&this->instructions[0].adr),
                sizeof(uint16_t));
    for(unsigned int idx = 0; idx < this->instructions.size(); ++idx)
    {
        outfile.write(reinterpret_cast<char*>
                (&this->instructions[idx].ins),
                sizeof(uint16_t));
        if(this->verbose)
        {
            std::cout << "Wrote instruction " << idx << "/" 
                << this->instructions.size() << "\r";
        }
    }

    if(this->verbose)
        std::cout << std::endl;
    outfile.close();

    return 0;
}

/*
 * load()
 */
int Program::load(const std::string& filename)
{
    std::ifstream infile;
    uint16_t num_records;
    uint16_t addr;

    this->instructions.clear();

    try {
        infile.open(filename, std::ios_base::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __FUNCTION__ << "] " << e.what() << std::endl;
        return -1;
    }

    infile.read(reinterpret_cast<char*>(&num_records), sizeof(uint16_t));
    if(num_records == 0)
    {
        std::cerr << "[" << __FUNCTION__ << "] no records in file " 
            << filename << std::endl;
        return -1;
    }
    // Load the first (only) address pointer 
    infile.read(reinterpret_cast<char*>(&addr), sizeof(uint16_t));

    Instr instr;
    for(unsigned int idx = 0; idx < num_records; ++idx)
    {
        infile.read(reinterpret_cast<char*>(&instr.ins), sizeof(uint16_t));
        //infile.read(reinterpret_cast<char*>(&instr.adr), sizeof(uint16_t));
        instr.adr = addr;
        this->instructions.push_back(instr);
        addr++;
    }
    infile.close();

    return 0;
}

void Program::setVerbose(const bool v)
{
    this->verbose = v;
}
bool Program::getVerbose(void) const
{
    return this->verbose;
}
