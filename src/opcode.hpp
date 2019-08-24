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

        std::string toString(void) const;
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
        Opcode null_op;
        std::vector<Opcode> op_list;
        
    public:
        OpcodeTable() {} 

        void add(const Opcode& o);
        /*
         * get()
         * Lookup opcode in table by mnemonic
         */
        Opcode get(const std::string& mnemonic) const;
        /*
         * get()
         * Look up opcode in table by code
         */
        Opcode get(const uint16_t opcode) const;
        /*
         * getIdx()
         * Return the n-th opcode in the table
         */
        Opcode getIdx(const unsigned int idx) const;
        /*
         * getMnemonic()
         * Return the mnemonic for a particular opcode.
         */
        std::string getMnemonic(const uint16_t opcode) const;
        void init(void);

        void print(void) const;

        // generic getters 
        unsigned int getNumOps(void) const;

};

#endif /*__OPCODE_HPP*/
