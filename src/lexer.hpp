/* LEXER
 * Lexer for Chip8 psuedo-assembler.
 *
 * Stefan Wong 2018
 */

#ifndef __LEXER_HPP
#define __LEXER_HPP

#define C8_DEBUG 1

#include <string>
#include "opcode.hpp"
#include "source.hpp"
#include "token.hpp"
#include "chip8.hpp"   // for opcodes

// Instruction codes - not quite opcodes
typedef enum instr_code{ 
    SYS, CLS, RET, JP, CALL, SE, SNE, LD, ADD, OR, AND, XOR, SUB, 
    SHR, SUBN, SHL, RND, DRW, SKP, SKNP
} instr_code;


// Instruction codes for Lexer 
const Opcode lex_instr_codes[] = {
    {SYS,  "SYS"},
    {CLS,  "CLS"},
    {RET,  "RET"},
    {JP,   "JP"},
    {CALL, "CALL"},
    {SE,   "SE"},
    {SNE,  "SNE"},
    {LD,   "LD"},
    {ADD,  "ADD"},
    {OR,   "OR"},
    {AND,  "AND"},
    {XOR,  "XOR"},
    {SUB,  "SUB"},
    {SHR,  "SHR"},
    {SUBN, "SUBN"},
    {SHL,  "SHL"},
    {RND,  "RND"},
    {DRW,  "DRW"},
    {SKP,  "SKP"},
    {SKNP, "SKNP"},
};



/* 
 * Lexer
 * Create a sequence of Chip-8 tokens.
 */
class Lexer
{
    private:
        bool verbose;
    private:
        std::string src;
        std::string filename;
        char* token_buf;
        char cur_char;
        unsigned int token_buf_size;
        unsigned int cur_pos;
        unsigned int cur_line;
        void alloc_mem(void);

    private:
        // opcodes
        OpcodeTable op_table;
        OpcodeTable instr_code_table;        // not quite opcodes....

        //OpcodeTable pseudo_op_table;
        void init_op_table(void);       // TODO: may remove ...
        void init_instr_code_table(void);

    private:
        // symbol table

    private:
        SourceInfo   source_info;
        LineInfo     line_info;
        unsigned int cur_addr;

    private:
        // Move through source
        bool exhausted(void) const;
        void advance(void);
        void skipWhitespace(void);
        void skipComment(void);
        void skipSeperators(void);

        void scanToken(void);
        bool isSymbol(void) const;
        bool isNumber(void) const;
        bool isDirective(void) const;
        bool isSpace(void) const;
        bool isComment(void) const;
        bool isMnemonic(void);

        // new token stuff
        //Token nextToken(void);

    private:
        // parsing 
        void parseToken(void);  // parse a single token
        void parseLine(void);   // parse an entire line
        void parseOpcode(void);
        void parseVxVy(void);
        void parseVxkk(void);
        bool checkArg(void);
        bool checkImm(void);

        void resolveLabels(void);

    public:
        Lexer();
        ~Lexer();
        Lexer(const Lexer& that) = delete;      // TODO ; implement later

        void lex(void);

        void loadFile(const std::string& filename);
        SourceInfo getSourceInfo(void) const;
        // Verbose mode
        void setVerbose(const bool v);
        bool getVerbose(void) const;
        
#if C8_DEBUG > 0
        // Debug routines 

#endif /*C8_DEBUG*/

};

#endif /*__LEXER_HPP*/
