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
        void dis_sevxvy(const uint16_t instr);
        void dis_sekk(const uint16_t instr);
        void dis_snevxvy(const uint16_t instr);
        void dis_snekk(const uint16_t instr);
        void dis_sne(const uint16_t instr);

        void dis_arith(const uint16_t instr);

    private:
        Program program;
        SourceInfo source;
        LineInfo cur_line;
        // TODO: ? 
        // uint16_t cur_instr?

    public:
        Disassembler();
        ~Disassembler();
        Disassembler(const Disassembler& that) = delete;

        // Load program
        int load(const std::string& filename);

        void disassemble(void);
        SourceInfo getSourceInfo(void) const;

        void setVerbose(const bool v);
        bool getVerbose(void) const;
};


#endif /*__DISASSEMBLER_HPP*/
