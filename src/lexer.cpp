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
    this->init_op_table();
    this->init_instr_code_table();
    this->verbose = false;
}

Lexer::~Lexer()
{
    delete[] this->token_buf;
}

//Lexer::Lexer(const Lexer& that) {} 

/*
 * alloc_mem()
 * Allocate token buffer memory
 */
void Lexer::alloc_mem(void)
{
    this->token_buf = new char[this->token_buf_size];
}

/*
 * init_op_table()
 * Set up the opcode table for the lexer 
 */
void Lexer::init_op_table(void)
{
    for(const Opcode& op : chip8_opcodes)
        this->op_table.add(op);
}

/*
 * init_instr_code_table()
 * Initialize a table of lexer instruction codes
 */
void Lexer::init_instr_code_table(void)
{
    for(const Opcode& code : lex_instr_codes)
        this->instr_code_table.add(code);
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
        this->cur_line = this->cur_line + 1;
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

/*
 * nextToken()
 * Get the next token in the stream
 */
void Lexer::nextToken(void)
{
    Opcode op;
    std::string token_str;

    this->scanToken();
    token_str = std::string(this->token_buf);
    this->instr_code_table.get(token_str, op);

    // Check for special tokens 
    if(token_str.length() == 1)
    { 
        std::cout << "[" << __FUNCTION__ << "] checking for special token (" << token_str << ")" << std::endl;
        if(token_str == "I" || token_str == "i")
        {
            this->cur_token.type = SYM_IREG;
            this->cur_token.val  = token_str[0];
        }
        else if(token_str == "B" || token_str == "b")
        {
            this->cur_token.type = SYM_BREG;
            this->cur_token.val  = token_str[0];
        }
        else if(token_str == "F" || token_str == "f")
        {
            this->cur_token.type = SYM_FREG;
            this->cur_token.val  = token_str[0];
        }
        else if(token_str == "K" || token_str == "k")
        {
            this->cur_token.type = SYM_KREG;
            this->cur_token.val  = token_str[0];
        }
        else
            goto TOKEN_LABEL;   // Assume one-letter label

        goto TOKEN_END;
    }

    if(token_str.length() == 2)
    {
        if(token_str == "DT")
        {
            this->cur_token.type = SYM_DT;
            this->cur_token.val  = token_str;
            goto TOKEN_END;
        }
        else if(token_str == "ST")
        {
            this->cur_token.type = SYM_ST;
            this->cur_token.val  = token_str;
            goto TOKEN_END;
        }
    }

    // Capture location I syntax
    if(token_str.length() == 3 && token_str == "[I]")
    {
        this->cur_token.type = SYM_LOC_I;
        this->cur_token.val  = token_str;
        goto TOKEN_END;
    }

    // Found an instruction
    if(op.mnemonic != "\0")
    {
        this->cur_token.type = SYM_INSTR;
        this->cur_token.val  = std::string(this->token_buf);
    }
    // Check if this is a register operand
    else if((token_str[0] == 'V' || token_str[0] == 'v'))
    {
        this->cur_token.type = SYM_REG;
        this->cur_token.val  = token_str;
    }
    // Check if this is an immediate 
    else if((token_str[0] == '0' && token_str[1] == 'x') || token_str[0] == '#')
    {
        this->cur_token.type = SYM_LITERAL;
        this->cur_token.val  = (token_str[0] == '#') ? (token_str.substr(1, token_str.length())) : token_str;
    }
    else
    {
        // Exhausted all options, must be an identifier
TOKEN_LABEL:
        this->cur_token.type = SYM_LABEL;
        this->cur_token.val  = token_str;
    }

TOKEN_END:
    if(this->verbose)
        std::cout << "[" << __FUNCTION__ << "] got token of type " << token_type_str[this->cur_token.type] << std::endl;
    //if(this->verbose)
    //{
    //    std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << this->cur_line 
    //        << ") got " << token_type_str[this->cur_token.type] << " token (" 
    //        << this->cur_token.val << ")" << std::endl;
    //}
}

/*
 * parseTwoArg()
 */
void Lexer::parseTwoArg(void)
{
    this->nextToken();
    if(this->cur_token.type == SYM_IREG)
    {
        this->line_info.reg_flags = LEX_IREG;
        this->nextToken();
        if(this->cur_token.type == SYM_REG)
        {
            this->line_info.vy = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        }
        else if(this->cur_token.type == SYM_LITERAL)
        {
            this->line_info.is_imm = true;
            this->line_info.kk = std::stoi(this->cur_token.val, nullptr, 16);
        }
        else if(this->cur_token.type == SYM_LABEL)
            this->line_info.symbol = this->cur_token.val;
        else
        {
            this->line_info.error = true;
            this->line_info.errstr = "Invalid operand 2 in " + token_type_str[this->cur_token.type] + " - must be register,immediate, or label";
        }
    }
    else
    {
        // first must be register arg
        if(this->cur_token.type != SYM_REG)
        {
            this->line_info.error = true;
            this->line_info.errstr = "Invalid operand 1 in " + token_type_str[this->cur_token.type] + " - must be register";
            goto ARG_END;
        }
        this->line_info.vx = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        this->nextToken();

        // Could be immediate or register 
        if(this->cur_token.type == SYM_REG)
            this->line_info.vy = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        else if(this->cur_token.type == SYM_LITERAL)
        {
            this->line_info.is_imm = true;
            this->line_info.kk = std::stoi(this->cur_token.val, nullptr, 16);
        }
        else
        {
            this->line_info.error = true;
            this->line_info.errstr = "Invalid operand 2 in " + token_type_str[this->cur_token.type] + " - must be register";
            goto ARG_END;
        }
    }

ARG_END:
    if(this->line_info.error && this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] (line " << this->cur_line
            << ") " << this->line_info.errstr << std::endl;
    }
}

/*
 * parseRegImm()
 */
void Lexer::parseRegImm(void)
{
    this->nextToken();
    if(this->cur_token.type == SYM_IREG)
    {
        this->line_info.reg_flags = LEX_IREG;
        this->nextToken();
        if(this->cur_token.type != SYM_REG)
        {
            this->line_info.error = true;
            this->line_info.errstr = "Invalid operand 2 in " + token_type_str[this->cur_token.type] + " - must be literal";
        }
        this->line_info.kk = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
    }
    else
    {
        // first must be register arg
        if(this->cur_token.type != SYM_REG)
        {
            this->line_info.error = true;
            this->line_info.errstr = "Invalid operand 1 in " + token_type_str[this->cur_token.type] + " - must be register";
            goto ARG_END;
        }
        this->line_info.vx = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        this->nextToken();

        // Second must be immediate
        if(this->cur_token.type != SYM_LITERAL)
        {
            this->line_info.error = true;
            this->line_info.errstr = "Invalid operand 2 in " + token_type_str[this->cur_token.type] + " - must be literal";
            goto ARG_END;
        }
        this->line_info.is_imm = true;
        this->line_info.kk = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        this->nextToken();
    }

ARG_END:
    if(this->line_info.error && this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] (line " << this->cur_line
            << ") " << this->line_info.errstr << std::endl;
    }
}

/*
 * parseAddr()
 */
void Lexer::parseAddr(void)
{
    std::string tok_str;

    this->nextToken();
    if(this->cur_token.type == SYM_LITERAL)
    {
        tok_str = std::string(this->cur_token.val);
        this->line_info.nnn = std::stoi(this->cur_token.val, nullptr, 16);
        this->line_info.is_imm = true;
    }
    else if(this->cur_token.type == SYM_LABEL)
    {
        this->line_info.symbol = this->cur_token.val;
    }
    else
    {
        this->line_info.error = true;
        this->line_info.errstr = "Argument must be immediate or label";
    }

    if(this->verbose && this->line_info.error)
    {
        std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << this->cur_line
            << ") " << this->line_info.errstr << std::endl;
    }
}

/*
 * parseVx()
 */
void Lexer::parseVx(void)
{
    this->nextToken();
    if(this->cur_token.type != SYM_REG)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid operand 1 in " + token_type_str[this->cur_token.type] + " - must be register";
        return;
    }
    this->line_info.vx = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
}

/*
 * parseWord()
 */
void Lexer::parseWord(void)
{
    this->nextToken();
    if(this->cur_token.type != SYM_LITERAL)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid operand 1 in " + token_type_str[this->cur_token.type] + " - must be literal";

    }
    this->line_info.nnn = std::stoi(this->cur_token.val, nullptr, 16);
}

/*
 * parseAdd()
 */
void Lexer::parseAdd(void)
{
    int argn = 0;

    this->nextToken();
    // first arg is either reg or I
    if(this->cur_token.type == SYM_IREG)
        this->line_info.reg_flags = LEX_IREG;
    else if(this->cur_token.type == SYM_REG)
        this->line_info.vx = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
    else
    {
        argn = 1;
        goto ARG_ERR;
    }
    
    // Second arg is either reg or imm
    this->nextToken();
    if(this->cur_token.type == SYM_REG)
        this->line_info.vy = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
    else if(this->cur_token.type == SYM_LITERAL)
    {
        this->line_info.kk = std::stoi(this->cur_token.val, nullptr, 16);
        this->line_info.is_imm = true;
    }
    else
    {
        argn = 2;
        goto ARG_ERR;
    }

ARG_ERR:
    if(argn > 0)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid LD argument " + std::to_string(argn) + " '" + this->cur_token.val + " : type " + 
            token_type_str[this->cur_token.type];
        if(this->verbose)
            std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
    }
}

/*
 * 
 * parseLD()
 */
void Lexer::parseLD(void)
{
    int argn = 0;

    this->nextToken();
    // First arg must be register or special registers
    if(this->cur_token.type == SYM_REG)
        this->line_info.vx = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
    else
    {
        //std::cout << "[" << __FUNCTION__ << "] got token type " 
        //    << token_type_str[this->cur_token.type] << std::endl;
        switch(this->cur_token.type)
        {
            case SYM_IREG:
                this->line_info.reg_flags = LEX_IREG;
                break;

            case SYM_BREG:
                this->line_info.reg_flags = LEX_BREG;
                break;

            case SYM_FREG:
                this->line_info.reg_flags = LEX_FREG;
                break;

            case SYM_KREG:
                this->line_info.reg_flags = LEX_KREG;
                break;

            case SYM_DT:
                this->line_info.reg_flags = LEX_DTREG;
                break;

            case SYM_ST:
                this->line_info.reg_flags = LEX_STREG;
                break;

            case SYM_LOC_I:
                this->line_info.reg_flags = LEX_IST;
                this->line_info.is_imm    = true;
                break;;

            default:
                argn = 1;
                goto ARG_ERR;
        }
    }

    // If first arg was register, second must be reg or literal, or symbol [I]
    if(this->cur_token.type == SYM_REG)
    {
        this->nextToken();
        if(this->cur_token.type == SYM_REG)
            this->line_info.vy = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        else if(this->cur_token.type == SYM_LITERAL)
        {
            this->line_info.kk = std::stoi(this->cur_token.val, nullptr, 16);
            this->line_info.is_imm = true;
        }
        else if(this->cur_token.type == SYM_LABEL)
            this->line_info.symbol = this->cur_token.val;
        else if(this->cur_token.type == SYM_LOC_I)
            this->line_info.reg_flags = LEX_ILD;
        else
        {
            argn = 2;
            goto ARG_ERR;
        }
    }
    else
    {
        // First arg was a special reg. Second arg must be a reg or symbol or DT
        this->nextToken();
        if(this->cur_token.type == SYM_REG)
            this->line_info.vy = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
        else if(this->cur_token.type == SYM_DT)
            this->line_info.is_imm = true;
        else if(this->cur_token.type == SYM_LABEL)
            this->line_info.symbol = cur_token.val;
        else
        {
            argn = 2;
            goto ARG_ERR;
        }
    }

ARG_ERR:
    if(argn > 0)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid LD argument " + std::to_string(argn) + " '" + this->cur_token.val + " : type " + 
            token_type_str[this->cur_token.type];
        if(this->verbose)
            std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
    }
}

/*
 * parseLine()
 */
void Lexer::parseLine(void)
{
    Opcode op;
    Symbol s;
    unsigned int line_num = 0;

    initLineInfo(this->line_info);
    this->nextToken();
    if(this->cur_token.type == SYM_LABEL)
    {
        this->line_info.is_label = true;
        // Add symbol to table, removing any trailing ':' characters 
        if(this->cur_token.val[this->cur_token.val.length() - 1] == ':')
            s.label = this->cur_token.val.substr(0, this->cur_token.val.length() -1);
        else
            s.label = this->cur_token.val;
        s.addr  = this->cur_addr;
        this->sym_table.add(s);
        this->line_info.label = s.label;
        // scan in the next token 
        this->nextToken();

        if(this->verbose)
        {
            std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << this->cur_line
                << ") found label " << s.label << std::endl;
        }
    }

    line_num = this->cur_line;

    if(this->cur_token.type == SYM_INSTR)
    {
        this->instr_code_table.get(this->cur_token.val, op);
        if(this->verbose)
        {
            std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << this->cur_line 
                << ") found instruction token " << this->cur_token.val << " (opcode x" 
                << std::hex << op.opcode << ", mnemonic "  << op.mnemonic << ")" << std::endl;
        }

        switch(op.opcode)
        {
            case LEX_ADD:
                this->parseAdd();
                break;
            // No args
            case LEX_CLS:
            case LEX_RET:
                break;

            case LEX_JP:
            case LEX_CALL:
                this->parseAddr();
                break;

            case LEX_LD:
                this->parseLD();
                break;

            case LEX_SE:
            case LEX_OR:
            case LEX_XOR:
            case LEX_SHL:
            case LEX_SHR:
            case LEX_SUB:
            case LEX_SUBN:
                this->parseTwoArg();
                break;

            case LEX_RND:
                this->parseRegImm();
                break;

            case LEX_SKP:
            case LEX_SKNP:
                this->parseVx();
                break;

            case LEX_DRW:
                this->parseTwoArg();
                // Should also be one more immediate  
                this->nextToken();
                if(this->cur_token.type != SYM_LITERAL)
                {
                    this->line_info.error = true;
                    this->line_info.errstr = "DRW argument 3 must be immediate";
                    if(this->verbose)
                    {
                        std::cout << "[" << __FUNCTION__ << "] (line " << this->cur_line
                            << ") " << this->line_info.errstr << std::endl;
                    }
                    goto LINE_END;
                    break;
                }
                this->line_info.kk = std::stoi(this->cur_token.val, nullptr, 16);
                break;

            case LEX_DW:
                this->parseWord();  
                break;

            default:
                this->line_info.error = true;
                this->line_info.errstr = "Invalid instruction " + this->cur_token.val;
                if(this->verbose)
                {
                    std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << 
                        this->cur_line << ") " << this->line_info.errstr << std::endl;
                }
                goto LINE_END;
        }
        this->line_info.opcode = op;
    }

LINE_END:
    //this->line_info.line_num = this->cur_line;
    this->line_info.line_num = line_num;
    this->line_info.addr     = this->cur_addr;
    this->cur_addr++;
}

/*
 * resolveLabels()
 */
void Lexer::resolveLabels(void)
{
    unsigned int idx;
    uint16_t label_addr;
    LineInfo line;

    if(this->verbose)
    {
        std::cout << this->sym_table.getNumSyms() << " symbols in table" << std::endl;
        for(unsigned s = 0; s < this->sym_table.getNumSyms(); ++s)
        {
            Symbol sym = this->sym_table.get(s);
            std::cout << "[" << std::hex << std::setw(4) << std::setfill('0') << sym.addr << "] ";
            std::cout << " " << sym.label << std::endl;
        }
    }

    for(idx = 0; idx < this->source_info.getNumLines(); ++idx)
    {
        line = this->source_info.get(idx);
        if(line.symbol != "\0")
        {
            label_addr = this->sym_table.getAddr(line.symbol);
            if(label_addr > 0)
            {
                if(line.opcode.mnemonic == "JP" || (line.reg_flags & LEX_IREG))
                    line.nnn = label_addr;
                else
                    line.kk = label_addr;
                this->source_info.update(idx, line);
                if(this->verbose)
                {
                    std::cout << "[" << __FUNCTION__ << "] updated label " 
                        << line.symbol << " with address " << std::hex 
                        << line.kk << std::endl;
                }
            }
        }
    }
}

/*
 * lex()
 * Lex the source 
 */
void Lexer::lex(void)
{
    this->cur_line = 1;
    this->cur_pos = 0;
    this->cur_addr = 0x200;     // TODO : proper address init..

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
        this->parseLine();
        this->source_info.add(this->line_info);
    }
    // Resolve symbols
    this->resolveLabels();
}

/*
 * loadFile()
 * Read source from the file given by filename.
 */
int Lexer::loadFile(const std::string& filename)
{
    std::ifstream infile(filename);
    std::string line;
    int status = 0;     // TODO: faliure checks, etc

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

    return status;
}

SourceInfo Lexer::getSourceInfo(void) const
{
    return this->source_info;
}
SymbolTable Lexer::getSymTable(void) const
{
    return this->sym_table;
}
unsigned int Lexer::getNumErr(void) const
{
    LineInfo line;
    unsigned int num_err = 0;
    for(unsigned int idx = 0; idx < this->source_info.getNumLines(); ++idx)
    {
        line = this->source_info.get(idx);
        if(line.error)
            num_err++;
    }

    return num_err;
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
