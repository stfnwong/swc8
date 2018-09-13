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
#include "chip8.hpp"   // for opcodes

// Instruction codes - not quite opcodes
typedef enum instr_code{ 
    LEX_SYS, LEX_CLS, LEX_RET, LEX_JP, LEX_CALL, LEX_SE, LEX_SNE, 
    LEX_LD, LEX_ADD, LEX_OR, LEX_AND, LEX_XOR, LEX_SUB, LEX_SHR, 
    LEX_SUBN, LEX_SHL, LEX_RND, LEX_DRW, LEX_SKP, LEX_SKNP,
    LEX_DW, LEX_DB
} instr_code;


// Instruction codes for Lexer 
const Opcode lex_instr_codes[] = {
    {LEX_SYS,  "SYS"},
    {LEX_CLS,  "CLS"},
    {LEX_RET,  "RET"},
    {LEX_JP,   "JP"},
    {LEX_CALL, "CALL"},
    {LEX_SE,   "SE"},
    {LEX_SNE,  "SNE"},
    {LEX_LD,   "LD"},
    {LEX_ADD,  "ADD"},
    {LEX_OR,   "OR"},
    {LEX_AND,  "AND"},
    {LEX_XOR,  "XOR"},
    {LEX_SUB,  "SUB"},
    {LEX_SHR,  "SHR"},
    {LEX_SUBN, "SUBN"},
    {LEX_SHL,  "SHL"},
    {LEX_RND,  "RND"},
    {LEX_DRW,  "DRW"},
    {LEX_SKP,  "SKP"},
    {LEX_SKNP, "SKNP"},
    {LEX_DW,   "DW"},
    {LEX_DB,   "DB"}
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
        SymbolTable  sym_table;
        unsigned int cur_addr;

    private:
        // Move through source
        bool exhausted(void) const;
        void advance(void);
        void skipWhitespace(void);
        void skipComment(void);
        void skipSeperators(void);
        void scanToken(void);
        bool isSpace(void) const;
        bool isComment(void) const;

    private:
        // new token stuff
        Token cur_token;
        void nextToken(void);

    private:
        // parsing 
        void parseLine(void);   // parse an entire line
        bool checkArg(void);
        bool checkImm(void);
        void resolveLabels(void);
        void parseTwoArg(void);
        void parseRegImm(void);
        void parseAddr(void);
        void parseVx(void);
        void parseWord(void);
        void parseAdd(void);
        void parseLD(void);

    public:
        Lexer();
        ~Lexer();
        Lexer(const Lexer& that) = delete;      // TODO ; implement later

        void lex(void);

        int loadFile(const std::string& filename);
        SourceInfo getSourceInfo(void) const;
        SymbolTable getSymTable(void) const;
        // Verbose mode
        void setVerbose(const bool v);
        bool getVerbose(void) const;
        unsigned int getNumErr(void) const;
        
#if C8_DEBUG > 0
        // Debug routines 

#endif /*C8_DEBUG*/

};

#endif /*__LEXER_HPP*/
