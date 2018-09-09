/* TOKEN 
 * Token objects, functions, etc 
 *
 * Stefan Wong 2018
 */

#ifndef __TOKEN_HPP
#define __TOKEN_HPP

#include <array>
#include <string>
#include <vector>

// Token types for lexer 
typedef enum TokenType{
    SYM_EOF,
    SYM_IDENT,
    SYM_INSTR,
    SYM_REG,
    SYM_LITERAL
} TokenType;

// Array for string conversion by index
const std::array<std::string, 5> token_type_str = {
    "EOF",
    "IDENT",
    "INSTR",
    "REG",
    "LITERAL"
};


/* 
 * LexToken
 * A token emitted by the lexer
 */
typedef struct 
{
    TokenType type;
    std::string val;
} Token;


/* 
 * TokenTable 
 */
class TokenTable
{
    private: 
        std::vector<Token> tokens;

    public:
        TokenTable();
        ~TokenTable();
        TokenTable(const TokenTable& that);
        
        // Modify table 
        void add(const Token& t);
        Token get(const unsigned int idx) const;
        std::string toString(void);
};

#endif /*__TOKEN_HPP*/
