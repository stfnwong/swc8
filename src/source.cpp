/* SOURCE
 * Code for source handling 
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include "source.hpp"

/*
 * Token
 */
Token::Token()
{
    this->type = SYM_NULL;
    this->val = "\0";
}

Token::Token(const TokenType& t, const std::string& v)
{
    this->type = t;
    this->val  = v;
}

bool Token::operator==(const Token& that) const
{
    if(this->type != that.type)
        return false;
    if(this->val != that.val)
        return false;
    return true;
}

bool Token::operator!=(const Token& that) const
{
    if(this->type == that.type)
        return false;
    if(this->val == that.val)
        return false;
    return true;
}

std::string Token::toString(void) const
{
    std::ostringstream oss;

    oss << "[" << token_type_str[this->type] << "] ";
    oss << " " << this->val;

    return oss.str();
}


/* 
 * TokenTable 
 */
TokenTable::TokenTable() {} 


void TokenTable::add(const Token& t)
{
    this->tokens.push_back(t);
}

Token TokenTable::get(const unsigned int idx) const
{
    if(idx < this->tokens.size())
        return this->tokens[idx];
    return this->null_token;
}

std::string TokenTable::toString(void)
{
    std::ostringstream oss;

    oss << "TODO: no string method at this time" << std::endl;

    return oss.str();
}

/*
 * SYMBOL
 */
Symbol::Symbol()
{
    this->addr = 0;
    this->label = "\0";
}

Symbol::Symbol(const uint16_t a, const std::string& l)
{
    this->addr = a;
    this->label = l;
}

bool Symbol::operator==(const Symbol& that) const
{
    if(this->addr != that.addr)
        return false;
    if(this->label != that.label)
        return false;
    return true;
}

bool Symbol::operator!=(const Symbol& that) const
{
    if(this->addr == that.addr)
        return false;
    if(this->label == that.label)
        return false;
    return true;
}


/*
 * SYMBOLTABLE 
 */
SymbolTable::SymbolTable() {} 


void SymbolTable::add(const Symbol& s)
{
    this->syms.push_back(s);
}

void SymbolTable::update(const unsigned int idx, const Symbol& s)
{
    //if(idx > this->syms.size())
    //    return;
    this->syms[idx] = s;
}

Symbol SymbolTable::get(const unsigned int idx) const
{
    // Worth doing bounds check?
    return this->syms[idx];
}

// TODO: Currently implemented as linear search,
// change to map or something
uint16_t SymbolTable::getAddr(const std::string& s) const
{
    uint16_t addr;
    unsigned int idx;

    for(idx = 0; idx < this->syms.size(); idx++)
    {
        if(s == this->syms[idx].label)
            return this->syms[idx].addr;
    }
    addr = 0;

    return addr;
}

void SymbolTable::init(void)
{
    this->syms.clear();
}

unsigned int SymbolTable::getNumSyms(void) const
{
    return this->syms.size();
}

void SymbolTable::dump(void)
{
    for(unsigned int idx = 0; idx < this->syms.size(); ++idx)
    {
        Symbol s = this->syms[idx];
        std::cout << "[" << s.label << "] : 0x" << std::hex << s.addr << std::endl;
    }
}


// LineInfo
LineInfo::LineInfo()
{
    this->init();
}

void LineInfo::init(void)
{
    this->symbol    = "\0";
    this->label     = "\0";
    this->errstr    = "\0";
    this->opcode    = {0x0, "INVALID"};
    this->line_num  = 0;
    this->addr      = 0;
    this->reg_flags = 0;
    this->op_flags  = 0;
    this->nnn       = 0;
    this->vx        = 0;
    this->vy        = 0;
    this->kk        = 0;
    this->is_label  = false;
    this->is_imm    = false;
    this->error     = false;
    this->is_directive = false;
}

bool LineInfo::operator==(const LineInfo& that) const
{
    if(this->symbol != that.symbol)
        return false;
    if(this->label != that.label)
        return false;
    if(this->opcode.opcode != that.opcode.opcode)
        return false;
    if(this->opcode.mnemonic != that.opcode.mnemonic)
        return false;
    if(this->line_num != that.line_num)
        return false;
    if(this->addr != that.addr)
        return false;
    if(this->vx != that.vx)
        return false;
    if(this->vy != that.vy)
        return false;
    if(this->kk != that.kk)
        return false;
    if(this->nnn != that.nnn)
        return false;
    if(this->is_label != that.is_label)
        return false;
    if(this->is_directive != that.is_directive)
        return false;
    if(this->error != that.error)
        return false;

    return true;
}

bool LineInfo::operator!=(const LineInfo& that) const
{
    return !(*this == that);
}


std::string LineInfo::toString(void) const
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    oss << "Line  Type   Addr  Mnemonic    Opcode  flags  Vx  Vy kk   nnn" << std::endl;
    //oss << "Line  Type   Addr  Mnemonic    Opcode  flags   arg1  arg2  arg3  imm  " << std::endl;

    oss << std::left << std::setw(6) << std::setfill(' ') << this->line_num;
    oss << "[";
    if(this->is_label)
        oss << "l";
    else
        oss << ".";
    if(this->is_directive)
        oss << "d";
    else
        oss << ".";
    if(this->is_imm)
        oss << "i";
    else
        oss << ".";
    oss << "] ";
    oss << std::right << "0x" << std::hex << std::setw(4) << std::setfill('0') << this->addr << " ";
    oss << std::left << std::setw(12) << std::setfill(' ') << this->opcode.mnemonic;
    oss << "0x" << std::right << std::hex << std::setw(4) << std::setfill('0') << this->opcode.opcode << "   ";
    // Insert flag chars
    oss << "...";
    // Registers
    oss << "  ";
    if(this->reg_flags & LEX_IREG)
        oss << "  I"; 
    else if(this->reg_flags & LEX_BREG)
        oss << "  B";
    else if(this->reg_flags & LEX_FREG)
        oss << "  F";
    else if(this->reg_flags & LEX_KREG)
        oss << "  K";
    else if(this->reg_flags & LEX_DTREG)
        oss << " DT";
    else if(this->reg_flags & LEX_STREG)
        oss << " ST";
    else if(this->reg_flags & LEX_IST)
        oss << " [I]";
    else
        oss << " V" << std::right << std::hex << std::setw(1) << this->vx;
    if(this->reg_flags & LEX_ILD)
        oss << " [I]";
    else
        oss << "  V" << std::right << std::hex << std::setw(1) << this->vy;
    oss << " 0x" << std::right << std::hex << std::setw(2) << std::setfill('0') << this->kk;
    oss << " 0x" << std::right << std::hex << std::setw(3) << std::setfill('0') << this->nnn;

    // (Next line) Text 
    oss << std::endl;
    oss << "Label [" << std::left << std::setw(16) << std::setfill(' ') << this->label << "] ";
    oss << "Symbol[" << std::left << std::setw(16) << std::setfill(' ') << this->symbol << "] ";

    oss << std::endl;
    
    return oss.str();
}


void printLineDiff(const LineInfo& a, const LineInfo& b)
{
    if(a.symbol != b.symbol)
        std::cout << "a.symbol [" << a.symbol << "] != b.symbol [" << b.symbol << "]" << std::endl;
    if(a.label != b.label)
        std::cout << "a.label [" << a.label << "] != b.label [" << b.label << "]" << std::endl;
    if(a.opcode.opcode != b.opcode.opcode)
        std::cout << "a.opcode [" << a.opcode.opcode << "] != b.opcode [" << b.opcode.opcode << "]" << std::endl;
    if(a.opcode.mnemonic != b.opcode.mnemonic)
        std::cout << "a.mnemonic [" << a.opcode.mnemonic << "] != b.mnemonic [" << b.opcode.mnemonic << "]" << std::endl;
    if(a.line_num != b.line_num)
        std::cout << "a.line_num [" << a.line_num << "] != b.line_num [" << b.line_num << "]" << std::endl;
    if(a.addr != b.addr)
        std::cout << "a.addr [" << a.addr << "] != b.addr [" << b.addr << "]" << std::endl;
    if(a.reg_flags != b.reg_flags)
        std::cout << "a.reg_flags [" << a.reg_flags << "] != b.reg_flags [" << b.reg_flags << "]" << std::endl;
    if(a.vx != b.vx)
        std::cout << "a.vx [" << a.vx << "] != b.vx [" << b.vx << "]" << std::endl;
    if(a.vy != b.vy)
        std::cout << "a.vy [" << a.vy << "] != b.vy [" << b.vy << "]" << std::endl;
    if(a.kk != b.kk)
        std::cout << "a.kk [" << a.kk << "] != b.kk [" << b.kk << "]" << std::endl;
    if(a.nnn != b.nnn)
        std::cout << "a.nnn [" << a.nnn << "] != b.nnn [" << b.nnn << "]" << std::endl;
    if(a.is_label != b.is_label)
        std::cout << "a.is_label [" << a.is_label << "] != b.is_label [" << b.is_label << "]" << std::endl;
    if(a.is_directive != b.is_directive)
        std::cout << "a.is_directive [" << a.is_directive << "] != b.is_directive [" << b.is_directive << "]" << std::endl;
    if(a.error != b.error)
        std::cout << "a.error [" << a.error << "] != b.error [" << b.error << "]" << std::endl;
}

/*
 * SOURCEINFO 
 */
SourceInfo::SourceInfo()
{
    this->error = false;
}

void SourceInfo::add(const LineInfo& l)
{
    this->line_info.push_back(l);
}

void SourceInfo::update(const unsigned int idx, const LineInfo& l)
{
    if(idx > this->line_info.size())
        return;
    this->line_info[idx] = l;
}

LineInfo SourceInfo::get(const unsigned int idx) const
{
    if(idx < this->line_info.size())
        return this->line_info[idx];
    else
    {
        LineInfo l;
        l.line_num = 0;
        
        return l;
    }
}

std::string SourceInfo::getStr(const unsigned int idx) 
{
    return this->line_info[idx].toString();
}

unsigned int SourceInfo::getLineNum(const unsigned int idx) const
{
    return this->line_info[idx].line_num;
}

unsigned int SourceInfo::getNumLines(void) const
{
    return this->line_info.size();
}

unsigned int SourceInfo::getNumError(void) const
{
    unsigned int num_err = 0;
    
    for(unsigned int idx = 0; idx < this->line_info.size(); idx++)
    {
        if(this->line_info[idx].error)
            num_err++;
    }

    return num_err;
}

/*
 * numInstance() 
 * 
 * Returns the number of times a particular opcode appears in
 * the source l.
 */
unsigned int SourceInfo::numInstance(const uint16_t& op) const
{
    unsigned int n = 0;
    unsigned int idx;

    for(idx = 0; idx < this->line_info.size(); idx++)
    {
        if(this->line_info[idx].opcode.opcode == op)
            n++;
    }

    return n;
}

/*
 * numInstance() 
 * 
 * Returns the number of times a particular mnemonic appears in
 * the source info.
 */
unsigned int SourceInfo::numInstance(const std::string& m) const
{
    unsigned int n = 0;
    unsigned int idx;
    // Could replace linear search here later...
    for(idx = 0; idx < this->line_info.size(); idx++)
    {
        if(this->line_info[idx].opcode.mnemonic == m)
            n++;
    }

    return n;
}

bool SourceInfo::hasError(void) const
{
    //return this->error;
    for(unsigned int idx = 0; idx < this->line_info.size(); ++idx)
    {
        if(this->line_info[idx].error)
            return true;
    }

    return false;
}

void SourceInfo::setError(const bool e) 
{
    this->error = e;
}

// Save/load data 
// TODO : may need a serialization library here 
int SourceInfo::write(const std::string& filename)
{
    int status = 0;
    //uint32_t num_records, sym_len, label_len;

    //num_records = this->line_info.size();

    return status;
}

// TODO : May need serialization library here
int SourceInfo::read(const std::string& filename)
{
    int status = 0;

    return status;
}

std::string SourceInfo::dumpErrors(void)
{
    std::ostringstream oss;

    for(unsigned int l = 0; l < this->line_info.size(); ++l)
    {
        if(this->line_info[l].error)
        {
            oss << "(line " << std::dec << this->line_info[l].line_num << ") ";
            oss << this->line_info[l].errstr << std::endl;
        }
    }

    return oss.str();
}
