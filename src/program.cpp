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



// ================ INSTR ================ //
Instr::Instr() : adr(0), ins(0) {} 

/*
 * Instr::==
 */
bool Instr::operator==(const Instr& that) const
{
    if(this->ins != that.ins)
        return false;
    if(this->adr != that.adr)
        return false;

    return true;
}

/*
 * Instr::!=
 */
bool Instr::operator!=(const Instr& that) const
{
    return !(*this == that);
}

/*
 * Instr::init()
 */
void Instr::init(void)
{
    this->ins = 0;
    this->adr = 0;
}

/*
 * Instr::toString()
 */
std::string Instr::toString(void) const
{
    std::ostringstream oss;

    oss <<  "[0x" << std::hex << std::setfill('0') 
        << std::setw(4) << this->adr << "] $" << this->ins;

    return oss.str();
}



// ================ PROGRAM ================ //
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
    if(idx >= 0 && idx < this->instructions.size())
        return this->instructions[idx];

    return Instr();
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

/*
 * numInstr()
 */
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
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
        return -1;
    }

    N = (uint16_t) this->instructions.size();
    outfile.write(reinterpret_cast<char*>(&N), sizeof(uint16_t));

    // Debug, remove 
    std::cout << "[" << __func__ << "] first address is " 
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
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
        return -1;
    }

    infile.seekg(0, std::ios::end);
    int file_len = infile.tellg();

    // divide by two here so that size is in 16-bit words
    num_records = infile.tellg() / 2;
    std::cout << "" << __func__ << "] " << num_records << " records in file (" 
        << file_len << " bytes) " << std::endl;

    infile.seekg(0, std::ios::beg);

   
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

    std::cout << "[" << __func__ << "] read " << this->instructions.size() 
        << " lines from file [" << filename << "]" << std::endl;

    // TODO: debug only - remove 
    std::cout << "[" << __func__ << "] instructions in buffer: " << std::endl;
    for(unsigned int idx = 0; idx < this->instructions.size(); ++idx)
    {
        std::cout << std::hex << std::setw(4) << std::setfill('0') 
            << this->instructions[idx].ins << " ";
    }
    std::cout << std::endl;



    return 0;
}

/*
 * writeObj()
 */
int Program::writeObj(const std::string& filename)
{
    std::ofstream outfile;
    uint8_t ub, lb;

    try {
        outfile.open(filename, std::ios_base::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
        return -1;
    }

    for(unsigned int i = 0; i < this->instructions.size(); ++i)
    {
        ub = (uint8_t) (this->instructions[i].ins >> 8) & 0xFF;
        lb = (uint8_t) (this->instructions[i].ins >> 0) & 0xFF;
        outfile.write(reinterpret_cast<char*>(&ub), sizeof(uint8_t));
        outfile.write(reinterpret_cast<char*>(&lb), sizeof(uint8_t));

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] Writing instruction "
                << i << "/" << this->instructions.size() << "\r";
        }
    }
    if(this->verbose)
        std::cout << std::endl << "...done" << std::endl;
    outfile.close();

    return 0;
}

/* 
 * readObj()
 */
int Program::readObj(const std::string& filename)
{
    std::ifstream infile;
    size_t num_bytes;
    uint16_t num_records;
    uint8_t ub, lb;

    try {
        infile.open(filename, std::ios_base::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
        return -1;
    }

    // Figure out how long the file is.
    infile.seekg(0, std::ios::end);
    num_bytes = infile.tellg(); 
    if(num_bytes % 4 != 0)
    {
        std::cerr << "[" << __func__ << "] contains " 
            << num_bytes << " bytes (" << num_bytes / 4 << " records)"
            << std::endl;
        return -1;
    }
    infile.seekg(0, std::ios::beg);
    num_records = num_bytes / 2;
    this->instructions.clear();

    Instr instr;
    for(unsigned int i = 0; i < num_records; ++i)
    {
        infile.read(reinterpret_cast<char*>(&ub), sizeof(uint8_t));
        infile.read(reinterpret_cast<char*>(&lb), sizeof(uint8_t));
        instr.ins = (ub << 8) | lb;
        instr.adr = 0x200 + i;
        // TODO : debug only, remove
        std::cout << std::hex << std::setw(4) << std::setfill('0')
            << instr.ins << " ";
        this->instructions.push_back(instr);
    }
    std::cout << std::endl;

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
