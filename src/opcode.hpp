/* OPCODE 
 * Opcode structures
 *
 * Stefan Wong 2018
 */

#ifndef __OPCODE_HPP
#define __OPCODE_HPP

#include <string>
#include <vector>
#include <cstdint>


/*
 * Opcode object
 */
struct Opcode
{
    uint16_t    opcode;
    std::string mnemonic;

    public:
        Opcode();
        Opcode(const uint16_t code, const std::string& m);

        bool operator==(const Opcode& that) const;
        bool operator!=(const Opcode& that) const;
};

/* 
 * OpcodeTable 
 *
 * Holds a list of Opcode structures.
 */
class OpcodeTable
{
    private:
        std::vector<Opcode> op_list;
        
    public:
        OpcodeTable() {} 

        void add(const Opcode& o);
        void get(const std::string& mnemonic, Opcode& o) const;
        void get(const uint16_t opcode, Opcode& o) const;
        std::string getMnemonic(const uint16_t opcode) const;
        void getIdx(const unsigned int idx, Opcode& o) const;
        void init(void);

        void print(void) const;

        // generic getters 
        unsigned int getNumOps(void) const;

};

#endif /*__OPCODE_HPP*/
