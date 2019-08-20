/* OPCODE 
 * Opcode structures
 *
 * Stefan Wong 2018
 */

#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "opcode.hpp"


/*
 * OPCODE
 */
Opcode::Opcode()
{
    this->opcode   = 0;
    this->mnemonic = "\0";
}

Opcode::Opcode(const uint16_t code, const std::string& m)
{
    this->opcode = code;
    this->mnemonic = m;
}

std::string Opcode::toString(void) const
{
    std::ostringstream oss;

    oss << this->mnemonic << std::hex << std::setw(4) << this->opcode;

    return oss.str();
}


bool Opcode::operator==(const Opcode& that) const
{
    if(this->opcode != that.opcode)
        return false;
    if(this->mnemonic != that.mnemonic)
        return false;
    return true;
}

bool Opcode::operator!=(const Opcode& that) const
{
    return !(*this == that);
}


// ======== OPCODE TABLE ======== //

void OpcodeTable::add(const Opcode& o)
{
    this->op_list.push_back(o);
}

// NOTE: could use a map here or something, but to be honest the search here 
// is so small that just walking the array is probably not much of a bottleneck
void OpcodeTable::get(const std::string& mnemonic, Opcode& o) const
{
    unsigned int idx;

    for(idx = 0; idx < this->op_list.size(); idx++)
    {
        if(mnemonic == this->op_list[idx].mnemonic)
        {
            o.opcode   = this->op_list[idx].opcode;
            o.mnemonic = this->op_list[idx].mnemonic;
            return;
        }
    }
    o.opcode = 0;
    o.mnemonic = "\0";
}

void OpcodeTable::get(const uint16_t opcode, Opcode& o) const
{
    // TODO : same as above but 'search by opcode'
    unsigned int idx;

    for(idx = 0; idx < this->op_list.size(); idx++)
    {
        if(opcode == this->op_list[idx].opcode)
        {
            o.opcode   = this->op_list[idx].opcode;
            o.mnemonic = this->op_list[idx].mnemonic;
            return;
        }
    }
    o.opcode = 0;
    o.mnemonic = "M_INVALID";
}

std::string OpcodeTable::getMnemonic(const uint16_t opcode) const
{
    // At some point maybe I should deal with all these linear scans..
    for(unsigned int idx = 0; idx < this->op_list.size(); ++idx)
    {
        if(opcode == this->op_list[idx].opcode)
            return this->op_list[idx].mnemonic;
    }

    return "OP_INVAILD";
}

void OpcodeTable::getIdx(const unsigned int idx, Opcode& o) const
{
    if(idx < this->op_list.size())
        o = this->op_list[idx];
}

void OpcodeTable::init(void)
{
    this->op_list.clear();
}

// Print the entire contents of the OpcodeTable to console
void OpcodeTable::print(void) const
{
    unsigned int idx;

    std::cout << this->op_list.size() << " instructions in table" << std::endl;
    for(idx = 0; idx < this->op_list.size(); idx++)
    {
        std::cout << "Op " << std::dec << std::setw(4) << std::setfill(' ') << 
            idx + 1 << " : [" << std::setw(5) << std::setfill(' ') << 
            this->op_list[idx].mnemonic.c_str() << "] 0x" << 
            std::hex << std::setw(2) << std::setfill('0') <<
            this->op_list[idx].opcode << std::endl;
    }
}

unsigned int OpcodeTable::getNumOps(void) const
{
    return this->op_list.size();
}
