/* TOKEN 
 * Token objects, functions, etc 
 *
 * Stefan Wong 2018
 */

#include <sstream>
#include "token.hpp"

// Note - this is almost the same as the Opcode table, and could
// possible be genericized.... at the moment the TokenTable is for
// the lexer and the OpcodeTable is for the assembler

TokenTable::TokenTable() {} 

TokenTable::~TokenTable()  {} 

TokenTable::TokenTable(const TokenTable& that)
{
    this->tokens.reserve(that.tokens.size());
    for(unsigned int t = 0; t < that.tokens.size(); ++t)
        this->tokens[t] = that.tokens[t];
}

void TokenTable::add(const Token& t)
{
    this->tokens.push_back(t);
}

Token TokenTable::get(const unsigned int idx) const
{
    // TODO : bounds check 
    return this->tokens[idx];
}

std::string TokenTable::toString(void)
{
    std::ostringstream oss;

    return oss.str();
}
