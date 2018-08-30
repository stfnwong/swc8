/* LEXER
 * Lexer for Chip8 psuedo-assembler.
 *
 * Stefan Wong 2018
 */


#ifndef __LEXER_HPP
#define __LEXER_HPP

#include <string>
#include "opcode.hpp"
#include "source.hpp"
#include "chip8.hpp"   // for opcodes

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
        //OpcodeTable pseudo_op_table;
        void init_op_table(void);

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

};

#endif /*__LEXER_HPP*/
