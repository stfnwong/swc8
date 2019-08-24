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

// ======== TOKEN ======== //

// Token types for lexer 
typedef enum TokenType{
    SYM_NULL,
    SYM_EOF,
    SYM_LABEL,
    SYM_INSTR,
    SYM_REG,
    SYM_LITERAL,
    SYM_IREG,
    SYM_BREG,
    SYM_FREG,
    SYM_DREG,
    SYM_KREG,
    SYM_DT,
    SYM_ST,
    SYM_LOC_I
} TokenType;

// Array for string conversion by index
static const std::vector<std::string> token_type_str = {
    "NULL",
    "EOF",
    "LABEL",
    "INSTR",
    "REG",
    "LITERAL",
    "I",
    "B",
    "F",
    "D",
    "K",
    "DT",
    "ST",
    "LOC_I" // [I]
};

/* 
 * LexToken
 * A token emitted by the lexer
 */
struct Token
{
    TokenType type;
    std::string val;

    public:
        Token();
        Token(const TokenType& t, const std::string& v);
        bool operator==(const Token& that) const;
        bool operator!=(const Token& that) const;
        std::string toString(void) const;
};


/* 
 * TokenTable 
 */
class TokenTable
{
    private: 
        Token null_token;
        std::vector<Token> tokens;

    // diable copying
    private:
        TokenTable(const TokenTable& that) = delete;
        TokenTable& operator=(const TokenTable& that) const = delete;

    public:
        TokenTable();
        
        // Modify table 
        void add(const Token& t);
        Token get(const unsigned int idx) const;
        std::string toString(void);
};

// ======== SYMBOL ======== //

struct Symbol
{
    uint16_t    addr;
    std::string label;

    public:
        Symbol();
        Symbol(const uint16_t a, const std::string& l);
        bool operator==(const Symbol& that) const;
        bool operator!=(const Symbol& that) const;
        //std::string toString(void) const;
};


class SymbolTable
{
    private:
        Symbol null_sym;
        std::vector<Symbol> syms;

    public:
        SymbolTable();

        void         add(const Symbol& s);
        void         update(const unsigned int idx, const Symbol& s);
        Symbol       get(const unsigned int idx) const;
        uint16_t     getAddr(const std::string& label) const;
        void         init(void);
        unsigned int getNumSyms(void) const;
        // debug 
        void         dump(void);
};

// Special register flags 
#define LEX_IREG  0x01
#define LEX_BREG  0x02
#define LEX_FREG  0x04
#define LEX_KREG  0x08
#define LEX_DTREG 0x10
#define LEX_STREG 0x20
#define LEX_IST   0x40  // for LD [I] Vx
#define LEX_ILD   0x80  // for LD Vx [I]

// Opcode flags 
#define LEX_OP_IMM   0x01
#define LEX_OP_LABEL 0x02
#define LEX_OP_DIR   0x04


// Chip 8 LineInfo structure 
struct LineInfo
{
    std::string  symbol;
    std::string  label;
    std::string  errstr;
    Opcode       opcode;
    unsigned int line_num;
    unsigned int addr;
    uint8_t      reg_flags;
    uint8_t      op_flags;
    uint16_t     nnn;
    uint16_t     vx;
    uint16_t     vy;
    uint16_t     kk;
    bool         is_label;
    bool         is_imm;
    bool         is_directive;
    bool         error;
    bool         dt;
    bool         st;

    public:
        LineInfo();

        void init(void);
        bool operator==(const LineInfo& that) const;
        bool operator!=(const LineInfo& that) const;
        std::string toString(void) const;
        std::string toDiffString(const LineInfo& that) const;
        std::string toInstrString(void) const;
};


/* 
 * SourceInfo
 * Object to hold information about assembly source
 */
class SourceInfo
{
    private:
        std::vector <LineInfo> line_info;
        bool error;
        
    public:
        SourceInfo();

        // Add/remove lines
        void         add(const LineInfo& l);
        void         update(const unsigned int idx, const LineInfo& l);
        LineInfo     get(const unsigned int idx) const;
        std::string  getStr(const unsigned int idx);
        unsigned int getLineNum(const unsigned int idx) const;
        unsigned int getNumLines(void) const;
        unsigned int getNumError(void) const;
        unsigned int numInstance(const uint16_t& op) const;
        unsigned int numInstance(const std::string& m) const;
        bool         hasError(void) const;
        void         setError(const bool e);

        std::string  dumpErrors(void);
}; 

#endif /*__SOURCE_HPP*/
