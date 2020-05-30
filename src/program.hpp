/* PROGRAM
 * ROutines for manipulating a Chip-8 program
 *
 * Stefan Wong 2018
 */

#ifndef __PROGRAM_HPP
#define __PROGRAM_HPP

#include <string>
#include <vector>
#include "source.hpp"

/*
 * Instr
 * Represents a single instruction in a chip-8 program
 */
struct Instr
{
    uint16_t adr;
    uint16_t ins;

    public:
        Instr();
        Instr(const Instr& that) = default;

        bool operator==(const Instr& that) const;
        bool operator!=(const Instr& that) const;

        void init(void);
        std::string toString(void) const;
};

/* 
 * Program
 * Object holding output of assembler.
 */
class Program
{
    private:
        bool verbose;
        std::vector<Instr> instructions;

    private:
        // string conversion
        std::string instr_to_string(const Instr& i);

    public:
        Program();
        ~Program();
        Program(const Program& that);

        void initProgram(void);
        /*
         * get()
         * Return the instruction at position idx of the 
         * instruction vector. If no such vector exists, 
         * an empty instruction is returned.
         */
        Instr get(const unsigned int idx) const;
        /*
         * getStr()
         * Return the string representation of the instruction
         * at position idx of the instruction vector. 
         */
        std::string getStr(const unsigned idx);
        /*
         * add()
         * Add an instruction to the end of the program.
         */
        void add(const Instr& i);

        /*
         * writeMem()
         * Add the value in val to the address adr in the program.
         * This method creates a new Instr structure containing the
         * value and address and appends it to the end of the 
         * instruction vector.
         */
        void writeMem(const uint16_t addr, const uint16_t val);

        /*
         * numInstr()
         * Returns the length of the instruction vector.
         */
        unsigned int numInstr(void) const;

        /*
         * writeObj()
         * Write program as a binary file to be read by an interpreter/
         * emulator.
         */
        int writeObj(const std::string& filename);
        /*
         * readObj()
         * Read a binary file.
         */
        int readObj(const std::string& filename);

        void setVerbose(const bool v);
        bool getVerbose(void) const;

};

#endif /*__PROGRAM_HPP*/
