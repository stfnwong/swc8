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
 * TokenTable 
 */
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

    oss << "TODO: no string method at this time" << std::endl;

    return oss.str();
}

/*
 * SYMBOLTABLE 
 */
SymbolTable::SymbolTable() {} 
SymbolTable::~SymbolTable() {} 

SymbolTable::SymbolTable(const SymbolTable& that)
{
    this->syms.reserve(that.syms.size());
    for(unsigned int s = 0; s < that.syms.size(); ++s)
        this->syms.push_back(that.syms[s]);
}

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
void initLineInfo(LineInfo& l)
{
    l.symbol    = "\0";
    l.label     = "\0";
    l.errstr    = "\0";
    l.opcode    = {0x0, "INVALID"};
    l.line_num  = 0;
    l.addr      = 0;
    l.reg_flags = 0;
    l.op_flags  = 0;
    l.nnn       = 0;
    l.vx        = 0;
    l.vy        = 0;
    l.kk        = 0;
    l.is_label  = false;
    l.is_imm    = false;
    l.error     = false;
    l.is_directive = false;
}

/*
 * compLineInfo()
 * Compare two LineInfo objects. errstr isn't checked here 
 */
bool compLineInfo(const LineInfo& a, const LineInfo& b)
{
    if(a.symbol != b.symbol)
        return false;
    if(a.label != b.label)
        return false;
    if(a.opcode.opcode != b.opcode.opcode)
        return false;
    if(a.opcode.mnemonic != b.opcode.mnemonic)
        return false;
    if(a.line_num != b.line_num)
        return false;
    if(a.addr != b.addr)
        return false;
    if(a.vx != b.vx)
        return false;
    if(a.vy != b.vy)
        return false;
    if(a.kk != b.kk)
        return false;
    if(a.nnn != b.nnn)
        return false;
    if(a.is_label != b.is_label)
        return false;
    if(a.is_directive != b.is_directive)
        return false;
    if(a.error != b.error)
        return false;

    return true;
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

SourceInfo::~SourceInfo() {} 

SourceInfo::SourceInfo(const SourceInfo& that)
{
    this->error = that.error;
    //this->line_info.reserve(that.line_info.size());
    for(unsigned int l = 0; l < that.line_info.size(); ++l)
        this->line_info.push_back(that.line_info[l]);
}

/* 
 * line_to_string
 * Pretty-print a LineInfo struct
 * TODO : LC3 version -> re-write for Chip8
 */
std::string SourceInfo::line_to_string(const LineInfo& l)
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    oss << "Line  Type   Addr  Mnemonic    Opcode  flags  Vx  Vy kk   nnn" << std::endl;
    //oss << "Line  Type   Addr  Mnemonic    Opcode  flags   arg1  arg2  arg3  imm  " << std::endl;

    oss << std::left << std::setw(6) << std::setfill(' ') << l.line_num;
    oss << "[";
    if(l.is_label)
        oss << "l";
    else
        oss << ".";
    if(l.is_directive)
        oss << "d";
    else
        oss << ".";
    if(l.is_imm)
        oss << "i";
    else
        oss << ".";
    oss << "] ";
    oss << std::right << "0x" << std::hex << std::setw(4) << std::setfill('0') << l.addr << " ";
    oss << std::left << std::setw(12) << std::setfill(' ') << l.opcode.mnemonic;
    oss << "0x" << std::right << std::hex << std::setw(4) << std::setfill('0') << l.opcode.opcode << "   ";
    // Insert flag chars
    oss << "...";
    // Registers
    oss << "  ";
    if(l.reg_flags & LEX_IREG)
        oss << "  I"; 
    else if(l.reg_flags & LEX_BREG)
        oss << "  B";
    else if(l.reg_flags & LEX_FREG)
        oss << "  F";
    else if(l.reg_flags & LEX_KREG)
        oss << "  K";
    else if(l.reg_flags & LEX_DTREG)
        oss << " DT";
    else if(l.reg_flags & LEX_STREG)
        oss << " ST";
    else if(l.reg_flags & LEX_IST)
        oss << " [I]";
    else
        oss << " V" << std::right << std::hex << std::setw(1) << l.vx;
    if(l.reg_flags & LEX_ILD)
        oss << " [I]";
    else
        oss << "  V" << std::right << std::hex << std::setw(1) << l.vy;
    oss << " 0x" << std::right << std::hex << std::setw(2) << std::setfill('0') << l.kk;
    oss << " 0x" << std::right << std::hex << std::setw(3) << std::setfill('0') << l.nnn;

    // (Next line) Text 
    oss << std::endl;
    oss << "Label [" << std::left << std::setw(16) << std::setfill(' ') << l.label << "] ";
    oss << "Symbol[" << std::left << std::setw(16) << std::setfill(' ') << l.symbol << "] ";

    oss << std::endl;
    
    return oss.str();
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
    return this->line_to_string(this->line_info[idx]);
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

void SourceInfo::printLine(const unsigned int idx)
{
    if(idx > this->line_info.size())
        return;
    std::cout << this->line_to_string(
            this->line_info[idx]);
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
