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
    if(this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] placed " 
            << std::dec << this->op_table.getNumOps() 
            << " opcodes in table" << std::endl;
    }
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

/*
 * isMnemonic()
 * Returns true if the current token is a valid mnemonic
 */
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

/*
 * parseVxVy()
 * Parse two register arguments 
 */
void Lexer::parseVxVy(void)
{
    uint16_t arg;
    std::string argstr;

    // arg1 
    this->scanToken();
    if(this->token_buf[0] == 'V' || 
       this->token_buf[0] == 'v' )
    {
        argstr = std::string(this->token_buf);
        arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
        this->line_info.arg1 = arg;
    }
    else
    {
        this->line_info.error  = true;
        this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
        if(this->verbose)
            std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
        return;
    }
    // arg2 
    this->scanToken();
    if(this->token_buf[0] == 'V' || 
       this->token_buf[0] == 'v' )
    {
        argstr = std::string(this->token_buf);
        arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
        this->line_info.arg2 = arg;
    }
    else
    {
        this->line_info.error  = true;
        this->line_info.errstr = "Invalid argument 2 (" + std::string(this->token_buf) + ")";
        if(this->verbose)
            std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
        return;
    }
}

void Lexer::parseVxkk(void)
{
    uint16_t arg;
    std::string argstr;

    // arg1 
    this->scanToken();
    if(this->token_buf[0] == 'V' || 
       this->token_buf[0] == 'v' )
    {
        argstr = std::string(this->token_buf);
        arg = std::stoi("0x" + argstr.substr(1, argstr.length()), nullptr, 16);
        this->line_info.arg1 = arg;
    }
    else
    {
        this->line_info.error  = true;
        this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
        if(this->verbose)
            std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
        return;
    }
    // arg2  (immediate)
    this->scanToken(); 
    if(this->token_buf[0] == '$' || 
       (this->token_buf[0] == '0' && this->token_buf[1] == 'x') || 
       this->token_buf[0] == 'x')
    {
        argstr = std::string(this->token_buf);
        arg = std::stoi("0x" + argstr.substr(1, argstr.length()), nullptr, 16);
        this->line_info.arg2 = arg;
        return;
    }
    // Assume label
    this->line_info.symbol = std::string(this->token_buf);
}

/*
 * parseToken()
 * Parse a token from the input stream
 */
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
                << std::string(this->token_buf) << ">" << std::endl;
        }
        this->parseOpcode();
        return;
    }

    // If we get to here this this is a label
    this->line_info.is_label = true;
    std::string label_str = std::string(this->token_buf);
    // Remove any trailing ':'
    if(label_str[label_str.length() - 1] == ':')
        this->line_info.label = label_str.substr(0, label_str.length() - 1);
    else
        this->line_info.label = label_str;
    if(this->verbose)
    {
        std::cout << "[" << __FUNCTION__ << "] got label <"
            << std::string(this->token_buf) << ">" << std::endl;
    }
}

// TODO: new structure for parseLine()
//
// 1) scanToken
// 2) check this->instr_code_table
// 3) If not in table, this is a label, move to next token
// 

void Lexer::nextToken(void)
{
    Opcode op;
    std::string token_str;

    this->scanToken();
    token_str = std::string(this->token_buf);
    this->instr_code_table.get(token_str, op);

    // Found an instruction
    if(op.mnemonic != "\0")
    {
        this->cur_token.type = SYM_INSTR;
        this->cur_token.val  = std::string(this->token_buf);
        return;
    }

    // Check if this is a register operand
    if((token_str[0] == 'V' || token_str[0] == 'v') && 
            isdigit(std::stoi(token_str.substr(1, 1), nullptr, 16)))
            //isdigit(std::stoi(std::string(token_str[1], nullptr, 16)))
    {
        this->cur_token.type = SYM_REG;
        this->cur_token.val  = token_str.substr(1, token_str.length());
        return;
    }

    // Check if this is an immediate 
    if((token_str[0] == '0' && token_str[1] == 'x') || token_str[0] == '#')
    {
        this->cur_token.type = SYM_LITERAL;
        this->cur_token.val  = (token_str[0] == '#') ? (token_str.substr(1, token_str.length())) : token_str;
        return;
    }

    // Check if the argument is the I register 
    if(token_str[0] == 'I')
    {
        this->cur_token.type = SYM_IREG;
        this->cur_token.val  = token_str[0];
        return;
    }

    // Exhausted all options, must be an identifier
    this->cur_token.type = SYM_LABEL;
    this->cur_token.val  = token_str;
}


/*
 * parseLine()
 */
void Lexer::parseLine(void)
{
    Opcode op;

    initLineInfo(this->line_info);
    this->nextToken();
    if(this->cur_token.type == SYM_LABEL)
    {
        this->line_info.is_label = true;
        this->line_info.symbol = cur_token.val;
        // scan in the next token 
        this->nextToken();
    }

    if(this->cur_token.type == SYM_INSTR)
    {
        this->instr_code_table.get(this->cur_token.val, op);
        switch(op.opcode)
        {
            case LEX_ADD:
                this->nextToken();
                if(this->cur_token.type == SYM_IREG)
                {
                    this->line_info.ireg = true;
                    this->nextToken();
                    if(this->cur_token.type != SYM_REG)
                    {
                        //error handling
                    }
                    this->line_info.arg1 = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
                }
                else
                {
                    if(this->cur_token.type != SYM_REG)
                    {
                        this->line_info.error = true;
                        this->line_info.errstr = "Invalid operand 1 in ADD - must be register";
                        if(this->verbose)
                        {
                            std::cout << "[" << __FUNCTION__ << "] (line" << this->cur_line 
                                << ") " << this->line_info.errstr << std::endl;
                        }
                        goto LINE_END;
                    }
                    this->line_info.arg1 = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
                    this->nextToken();
                    // Could be immediate or register 
                    if(this->cur_token.type == SYM_REG)
                        this->line_info.arg2 = std::stoi(this->cur_token.val.substr(1,1), nullptr, 16);
                    else if(this->cur_token.type == SYM_LITERAL)
                    {
                        this->line_info.arg2 = std::stoi(
                                this->cur_token.val.substr(1, this->cur_token.val.length()), nullptr, 16);
                    }
                    else
                    {
                        this->line_info.error = true;
                        this->line_info.errstr = "Invalid operand 2 in ADD";
                        if(this->verbose)
                        {
                            std::cout << "[" << __FUNCTION__ << "] (line " << this->cur_line
                                << ") " << this->line_info.errstr << std::endl;
                        }
                        goto LINE_END;
                    }
                }
                    
                break;

            default:
                this->line_info.error = true;
                this->line_info.errstr = "Unknown opcode " + op.opcode;
                if(this->verbose)
                {
                    std::cout << "[" << __FUNCTION__ << "] (line " << std::dec << 
                        this->cur_line << ") " << this->line_info.errstr << std::endl;
                }
                goto LINE_END;
        }
    }

LINE_END:
    this->line_info.line_num = this->cur_line;
    this->line_info.addr     = this->cur_addr;
    this->cur_addr++;
}


//void Lexer::parseLine(void)
//{
//    initLineInfo(this->line_info);
//
//    // Get the first token
//    this->parseToken();
//    if(this->line_info.error)
//    {
//        std::cerr << "[" << __FUNCTION__ << "] error parsing line "
//            << std::dec << this->line_info.line_num << " (" 
//            << this->line_info.errstr << ")" << std::endl;
//        goto LINE_END;
//    }
//
//    // If we have a label, then try to parse the 'actual' token(s)
//    if(this->line_info.is_label)
//    {
//        this->skipWhitespace();
//        this->parseToken();
//    }
//
//LINE_END:
//    this->line_info.line_num = this->cur_line;
//    this->line_info.addr     = this->cur_addr;
//    this->cur_addr++;
//}

/*
 * checkArg()
 */
bool Lexer::checkArg(void)
{
    if(this->token_buf[0] == 'V' || 
       this->token_buf[0] == 'v' )
        return true;

    return false;
}

/*
 * checkImm()
 * Check if this is a byte kk
 */
bool Lexer::checkImm(void)
{
    if(this->token_buf[0] == '$' || 
       (this->token_buf[0] == '0' && this->token_buf[1] == 'x') || 
       this->token_buf[0] == 'x')
        return true;

    return false;
}

/* 
 * parseOpcode()
 * Parse an opcode
 */
void Lexer::parseOpcode(void)
{
    Opcode op;
    uint16_t arg;
    std::string argstr;

    // mnemonic lookup
    this->op_table.get(std::string(this->token_buf), op);
    if(op.mnemonic == "M_INVALID")
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid opcode " + op.mnemonic;
        if(this->verbose)
            std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
        return;
    }

    this->line_info.opcode = op;
    this->line_info.addr   = this->cur_addr;
    switch(op.opcode)
    {

        case C8_SE:
            // first arg must be a register number 
            this->scanToken();
            if(!this->checkArg())
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }
            argstr = std::string(this->token_buf);
            arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
            this->line_info.arg1 = arg;

            // If the second argument is an immediate, then this is SEVxkk, otherwise its SEVxVy
            this->scanToken();
            if(this->isNumber())
            {
                argstr = std::string(this->token_buf);
                arg = std::stoi(argstr, nullptr, 16);
                this->line_info.imm = arg;
                this->line_info.opcode.opcode = C8_SEKK;
            }
            if(!this->checkArg())
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 2 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }
            argstr = std::string(this->token_buf);
            arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
            this->line_info.arg2 = arg;
            this->line_info.opcode.opcode = C8_SEVX;

            break;

        case C8_SNE:
            this->scanToken();
            // First arg must be register
            if(!this->checkArg())
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }
            argstr = std::string(this->token_buf);
            arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
            this->line_info.arg1 = arg;
            // If second argument is immediate, we have SNEVxkk, otherwise SNEVx

            break;

        case C8_LDVxkk:
            this->scanToken();
            argstr = std::string(this->token_buf);
            if(argstr[0] == 'I')
            {
                this->line_info.ireg = 1;
            }
            else if(argstr[0] == 'F')
            {
                std::cout << "LD F (TODO)" << std::endl;
            }
            else
            {
                // First arg is register 
                if(this->checkArg())
                {
                    argstr = std::string(this->token_buf);
                    arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
                    this->line_info.arg1 = arg;
                }
                else
                {
                    this->line_info.error = true;
                    this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
                    if(this->verbose)
                        std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                    return;
                }
            }
            // Second arg might be imm or symbol...
            this->scanToken();
            if(this->checkImm())
            {
                // TODO : allow for decimal immediates?
                arg = std::stoi(std::string(this->token_buf), nullptr, 16);
                this->line_info.is_imm = true;
                this->line_info.imm    = arg;
            }
            // Assume this is a symbol and check later 
            else
                this->line_info.symbol = std::string(this->token_buf);

            break;

        case C8_OR:
            // first arg is a register 
            this->scanToken();
            if(!this->checkArg())
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }
            argstr = std::string(this->token_buf);
            arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
            this->line_info.arg1 = arg;
            // second arg is also a register
            this->scanToken();
            if(!this->checkArg())
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 2 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }
            argstr = std::string(this->token_buf);
            arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
            this->line_info.arg2 = arg;

            break;

        // TODO : since we don't know what the opcode is until after we parse the args 
        // we may need to use a new enum for this
        case C8_ADD: //8ry4
            this->scanToken();
            // First arg must be a register 
            if(!this->checkArg())
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }
            argstr = std::string(this->token_buf);
            arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
            this->line_info.arg1 = arg;
            // Second arg may be an immediate
            this->scanToken();
            if(this->checkImm())
            {
                arg = std::stoi(std::string(this->token_buf));
                this->line_info.is_imm = true;
                this->line_info.imm    = arg;
            }
            else if(this->checkArg())
            {
                argstr = std::string(this->token_buf);
                arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
                this->line_info.arg2 = arg;
            }
            else
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid argument 1 (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }

            break;

        case C8_JP:
            // Could be an immediate
            this->scanToken();
            if(this->checkImm())
            {
                argstr = std::string(this->token_buf);
                arg = std::stoi(argstr.substr(1, argstr.length()), nullptr, 16);
                this->line_info.is_imm = true;
                this->line_info.imm = arg;
                return;
            }
            // Assume that this is symbol we will resolve later 
            this->line_info.symbol = std::string(this->token_buf);

            break;

        case C8_DRW:
            // Get first two tokens
            this->parseVxVy();
            if(this->line_info.error)
                return;
            // we have one more immediate 
            this->scanToken();
            if(this->checkImm())
            {
                arg = std::stoi(std::string(this->token_buf));
                this->line_info.is_imm = true;
                this->line_info.imm    = arg;
            }
            else
            {
                this->line_info.error = true;
                this->line_info.errstr = "Invalid immediate (" + std::string(this->token_buf) + ")";
                if(this->verbose)
                    std::cout << "[" << __FUNCTION__ << "] " << this->line_info.errstr << std::endl;
                return;
            }

            break;


//#define C8_LD   0x8000
//#define C8_OR   0x8001
//#define C8_AND  0x8002
//#define C8_XOR  0x8003
//#define C8_ADD  0x8004
//#define C8_SUB  0x8005
//#define C8_SHR  0x8006
//#define C8_SUBN 0x8007
//#define C8_SHL  0x800E
//#define C8_SNE  0x9000
//#define C8_LDI      0xA000
//#define C8_JP       0xB000

    }

}


void Lexer::lex(void)
{
    this->cur_line = 1;
    this->cur_pos = 0;
    this->cur_addr = 0x150;     // TODO : proper address init..

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
            // After symbols are parsed, add to source
            this->source_info.add(this->line_info);
        }
    }
}

/*
 * loadFile()
 * Read source from the file given by filename.
 */
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
