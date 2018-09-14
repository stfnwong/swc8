/*
 * Disassembler
 * Chip-8 disassembler
 *
 * Stefan Wong 2018
 */

#ifndef __DISASSEMBLER_HPP
#define __DISASSEMBLER_HPP

#include <cstdint>
#include <string>
#include "program.hpp"
#include "source.hpp"

class Disassembler
{
    private:
        bool verbose;

    private:
        // Args disassembly
        inline uint8_t dis_vx(const uint16_t instr);
        inline uint8_t dis_vy(const uint16_t instr);
        inline uint8_t dis_kk(const uint16_t instr);
        inline uint16_t dis_nnn(const uint16_t instr);

        // instruction disassembly
        // LD 
        void dis_ldvxvy(void);
        void dis_ldkk(void);
        void dis_ldi(void);

        // SE 
        void dis_sevxvy(void);
        void dis_sekk(void);
        void dis_snevxvy(void);
        void dis_snekk(void);
        void dis_sne(void);

        // Arithmetic instructions 
        void dis_arith(void);
        void dis_skp(void);
        void dis_ld_special(void);

    private:
        Program program;
        SourceInfo source;
        LineInfo cur_line;
        uint16_t cur_instr;

    public:
        Disassembler();
        ~Disassembler();
        Disassembler(const Disassembler& that) = delete;

        // Load program
        int load(const std::string& filename);

        void disassemble(void);
        SourceInfo getSourceInfo(void) const;
        Program    getProgram(void) const;

        void setVerbose(const bool v);
        bool getVerbose(void) const;
};


#endif /*__DISASSEMBLER_HPP*/
