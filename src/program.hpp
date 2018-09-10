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

void initInstr(Instr& i);

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
        Instr get(const unsigned int idx) const;
        std::string getStr(const unsigned idx);
        void add(const Instr& i);
        void writeMem(const uint16_t addr, const uint16_t val);

        unsigned int numInstr(void) const;

        void setVerbose(const bool v);
        bool getVerbose(void) const;

};

#endif /*__PROGRAM_HPP*/
