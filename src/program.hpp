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

typedef struct
{
    uint16_t adr;
    uint16_t ins;
} Instr;

/* 
 * initInstr()
 * Initialize the instruction object i
 */
void initInstr(Instr& i);

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
         * save()
         * 
         * Inputs: std::string filename
         * Write the program binary to the file given by filename
         */
        int save(const std::string& filename);
        /* 
         * load()
         * Load the object file with name filename into memory as 
         * a Program object.
         */
        int load(const std::string& filename);

        int writeObj(const std::string& filename);
        int readObj(const std::string& filename);

        void setVerbose(const bool v);
        bool getVerbose(void) const;

};

#endif /*__PROGRAM_HPP*/
