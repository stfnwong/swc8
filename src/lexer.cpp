/* LEXER
 * Lexer for Chip8 psuedo-assembler.
 *
 * Stefan Wong 2018
 */

#include <fstream>
#include <iostream>
#include <iomanip>
#include "lexer.hpp"

Lexer::Lexer()
{
    this->token_buf_size = 256;
    this->cur_pos = 0;
    this->alloc_mem();
}

Lexer::~Lexer()
{
    delete[] this->token_buf;
}

//Lexer::Lexer(const Lexer& that) {} 


void Lexer::alloc_mem(void)
{
    this->token_buf = new char[this->token_buf_size];
}

void Lexer::init_op_table(void)
{
    for(const Opcode& op : chip8_opcodes)
        this->op_table.add(op);
    if(this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] placed " 
            << std::dec << this->op_table.getNumOps() 
            << " opcodes in table" << std::endl;
    }
}

/* 
 * skipWhitespace()
 * Eat a sequence of continuous whitespace characters
 */
void Lexer::skipWhitespace(void) 
{
    while(!this->exhausted())
    {
        if(this->cur_char == ' '  || 
           this->cur_char == '\n' || 
           this->cur_char == '\t')
            this->advance();
        else
            break;
    }
}

void Lexer::skipComment(void)
{
    while(this->cur_char != '\n')
        this->advance();
}

void Lexer::skipSeperators(void)
{
    while(!this->exhausted())
    {
        if(this->cur_char == ',' ||
           this->cur_char == ':' ||
           this->cur_char == ';')
            this->advance();
        else
            break;
    }
}

bool Lexer::isSymbol(void) const
{
    return (isalnum(toupper(this->cur_char)));
}
bool Lexer::isNumber(void) const
{
    return (isdigit(toupper(this->cur_char)));
}
bool Lexer::isDirective(void) const 
{
    return (this->cur_char == '.' || this->token_buf[0] == '.');
}
bool Lexer::isSpace(void) const
{
    return (this->cur_char == ' '  || 
            this->cur_char == '\t' ||
            this->cur_char == '\n') ? true : false;
}
bool Lexer::isComment(void) const
{
    return (this->cur_char == ';');
}

bool Lexer::isMnemonic(void)
{
    if(this->token_buf[0] == '\0')
        return false;
    Opcode op;
    this->op_table.get(std::string(this->token_buf), op);

    return (op.mnemonic != "M_INVALID") ? true : false;
}

/*
 * scanToken()
 * Scan a complete token into the token buffer
 */
void Lexer::scanToken(void)
{
    unsigned int idx = 0;
    this->skipWhitespace();     // eat any leading whitespace 
    this->skipSeperators();     // eat any seperators that might be left
    while(idx < (this->token_buf_size-1))
    {
        if(this->cur_char == ' ')       // end 
            break;
        if(this->cur_char == '\n')      // newline
            break;
        if(this->cur_char == ';')       // comment
            break;
        if(this->cur_char == ',')       // seperator
            break;
        this->token_buf[idx] = this->cur_char;
        this->advance();
        idx++;
    }
    this->token_buf[idx] = '\0';
    // If we are on a seperator now, advance the source pointer 
    if(this->cur_char == ',')
        this->advance();

    if(this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << 
            this->cur_line << ") : token_buf contains <" << 
            std::string(this->token_buf) << "> " << std::endl;
    }
}

/*
 * advance()
 * Advance the current position pointer, checking for end of sequence
 * and updating the current line pointer.
 */
void Lexer::advance(void)
{
    this->cur_pos++;
    this->cur_char = this->src[this->cur_pos];
    if(this->cur_pos >= this->src.size())
        this->cur_char = '\0';
    if(this->cur_char == '\n')
        this->cur_line++;
}

/*
 * exhausted()
 * Returns true if there are no more characters in the source file
 */
bool Lexer::exhausted(void) const
{
    return (this->cur_char == '\0' ||
            this->cur_pos >= this->src.size()) ? true : false;
}

void Lexer::parseToken(void)
{
    this->scanToken();
    // Check directive 

    // Check pseudo-op

    // Check mnemonic 
    if(this->isMnemonic())
    {
        if(this->verbose)
        {
            std::cout << "[" << __FUNCTION__ << "] got mnemonic token <"
                << std::string(this->token_buf) << std::endl;
        }
        return;
    }

    // If we get to here this this is a label
    this->line_info.is_label = true;
    this->line_info.label    = std::string(this->token_buf);
    if(this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] got label <"
            << std::string(this->token_buf) << std::endl;
    }
}

void Lexer::parseLine(void)
{
    initLineInfo(this->line_info);

    // Get the first token
    this->parseToken();
    if(this->line_info.error)
    {
        std::cerr << "[" << __FUNCTION__ << "] error parsing line "
            << std::dec << this->line_info.line_num << " (" 
            << this->line_info.errstr << ")" << std::endl;
        goto LINE_END;
    }

    // If we have a label, then try to parse the 'actual' token(s)
    if(this->line_info.is_label)
    {
        this->skipWhitespace();
        this->parseToken();
    }

LINE_END:
    this->line_info.addr = this->cur_addr;
    this->cur_addr++;
}


void Lexer::lex(void)
{
    this->cur_line = 1;
    this->cur_pos = 0;

    while(!this->exhausted())
    {
        // eat spaces 
        if(this->isSpace())
        {
            this->advance();
            continue;
        }
        // eat comments 
        if(this->isComment())
        {
            this->skipComment();
            continue;
        }
        if(this->isSymbol())
        {
            if(this->verbose)
            {
                std::cout << "[" << __FUNCTION__ << "] (line " 
                    << std::dec << this->cur_line << ") found symbol"
                    << std::endl;
            }
            this->parseLine();
        }
        
        // After symbols are parsed, add to source
        this->source_info.add(this->line_info);
    }

}

void Lexer::loadFile(const std::string& filename)
{
    std::ifstream infile(filename);
    std::string line;

    // save the filename
    this->filename = filename;
    while(std::getline(infile, line))
    {
        this->src += line;
        this->src.push_back('\n');
    }

    if(this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] read " << 
            std::dec << this->src.length() << " characters from file ["
            << filename << "]" << std::endl;
    }

    infile.close();
    this->src.push_back('\0');
    this->cur_char = this->src[0];

}


SourceInfo Lexer::getSourceInfo(void) const
{
    return this->source_info;
}

// Verbose 
void Lexer::setVerbose(const bool v)
{
    this->verbose = v;
}

bool Lexer::getVerbose(void) const
{
    return this->verbose;
}
