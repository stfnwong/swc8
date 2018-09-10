/* ASSEMBLER 
 * Chip8 psuedo-assembler 
 *
 * Stefan Wong 2018
 */


#ifndef __ASSEMBLER_HPP
#define __ASSEMBLER_HPP

#include <string>
#include "program.hpp"
#include "source.hpp"

class Assembler
{
    private:
        bool verbose;

    private:
        // Lexer output
        SourceInfo source;

    private:
        // Binary
        Program program;

    private:
        inline uint8_t get_reg(const uint16_t r);
        inline uint16_t insert_vxvy(const uint16_t vx, const uint16_t vy);
        // Assembly routines
        void asm_add(const LineInfo& l); 
        void asm_and(const LineInfo& l); 
        void asm_ld(const LineInfo& l);
        void asm_se(const LineInfo& l);

    public:
        Assembler();
        ~Assembler();
        Assembler(const Assembler& that);

        void assemble(void);

        int readFile(const std::string& filaneme);
        void loadSource(const SourceInfo& s);

        Program getProgram(void) const;


        // Set verbosity
        void setVerbose(const bool v);
        bool getVerbose(void) const;

};

#endif /*__ASSEMBLER_HPP*/
