/* SOURCE
 * Code for source handling 
 *
 * Stefan Wong 2018
 */

#ifndef __SOURCE_HPP
#define __SOURCE_HPP

#include <string>
#include <cstdint>
#include "opcode.hpp"

typedef struct 
{
    uint16_t    addr;
    std::string label;
}Symbol;

class SymbolTable
{
    private:
        std::vector<Symbol> syms;
    public:
        SymbolTable();
        ~SymbolTable();
        void         add(const Symbol& s);
        void         update(const unsigned int idx, const Symbol& s);
        Symbol       get(const unsigned int idx) const;
        uint16_t     getAddr(const std::string& label) const;
        void         init(void);
        unsigned int getNumSyms(void) const;
        // debug 
        void         dump(void);
};

// Chip 8 LineInfo structure 
typedef struct{
    std::string  symbol;
    std::string  label;
    std::string  errstr;
    Opcode       opcode;
    unsigned int line_num;
    unsigned int addr;
    uint16_t     jmp_addr;
    uint16_t     arg1;
    uint16_t     arg2;
    uint16_t     ireg;
    uint16_t     imm;
    bool         is_label;
    bool         is_imm;
    bool         is_directive;
    bool         error;
} LineInfo;

/*
 * initLineInfo()
 * Reset a lineinfo struct
 */
void initLineInfo(LineInfo& l);
/*
 * compLineInfo()
 * Compare two LineInfo structs
 */
bool compLineInfo(const LineInfo& a, const LineInfo& b);
void printLineDiff(const LineInfo& a, const LineInfo& b);

/* 
 * SourceInfo
 * Object to hold information about assembly source
 */
class SourceInfo
{
    private:
        std::vector <LineInfo> line_info;
        std::string line_to_string(const LineInfo& l);
        bool error;
        
    public:
        SourceInfo();
        ~SourceInfo();
        // Add/remove lines
        void         add(const LineInfo& l);
        void         update(const unsigned int idx, const LineInfo& l);
        LineInfo     get(const unsigned int idx) const;
        unsigned int getLineNum(const unsigned int idx) const;
        unsigned int getNumLines(void) const;
        unsigned int getNumError(void) const;
        unsigned int numInstance(const uint16_t& op) const;
        unsigned int numInstance(const std::string& m) const;
        bool         hasError(void) const;
        void         setError(const bool e);

        // Save/load data
        int          write(const std::string& filename);
        int          read(const std::string& filename);

        // String / display 
        void         printLine(const unsigned int idx);
}; 

#endif /*__SOURCE_HPP*/
