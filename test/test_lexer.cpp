/* TEST_LEXER 
 * Tests for Chip8 psuedo-assembler 
 *
 * Stefan Wong 2018
 */

#include <vector>
#include <string>
#include <gtest/gtest.h>
#include "lexer.hpp"


class TestLexer : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// Generate expected source info for draw.asm
SourceInfo get_draw_expected_source_info(void)
{
    SourceInfo info;
    LineInfo line;

    // Note that the "opcode" field should really be thought of as
    // the lexer instruction code field
    // Line 1 
    initLineInfo(line);
    line.line_num        = 3;
    line.addr            = 0x200;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0x1;
    line.kk              = 0x08;
    info.add(line);
    // Line 2 
    initLineInfo(line);
    line.line_num        = 4;
    line.addr            = 0x201;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0x2;
    line.kk              = 0x00;
    info.add(line);
    // Line 3 
    initLineInfo(line);
    line.line_num        = 7;
    line.addr            = 0x202;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0xA;
    line.kk              = 0x05;
    info.add(line);
    // Line 4 
    initLineInfo(line);
    line.line_num        = 8;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    info.add(line);
    // Line 5 
    initLineInfo(line);
    line.line_num        = 11;
    line.addr            = 0x204;
    line.opcode.opcode   = 0x8;    
    line.opcode.mnemonic = "ADD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    info.add(line);
    // Line 6 
    initLineInfo(line);
    line.line_num        = 12;
    line.addr            = 0x205;
    line.symbol          = "Line";
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vy              = 0x01;
    line.ireg            = true;
    info.add(line);
    // Line 7 
    initLineInfo(line);
    line.line_num        = 13;
    line.addr            = 0x206;
    line.opcode.opcode   = 0xB;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 8 
    initLineInfo(line);
    line.line_num        = 14;
    line.addr            = 0x207;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "SE";
    line.vx              = 0xB;
    line.vy              = 0x1;
    info.add(line);
    // Line 9 
    initLineInfo(line);
    line.line_num        = 15;
    line.addr            = 0x208;
    line.symbol          = "Draw_Column_C";
    line.opcode.opcode   = 0x3;    
    line.opcode.mnemonic = "JP";
    info.add(line);
    // Line 10 
    initLineInfo(line);
    line.line_num        = 16;
    line.addr            = 0x209;
    line.symbol          = "Draw_H";
    line.opcode.opcode   = 0x3;    
    line.opcode.mnemonic = "JP";
    info.add(line);
    // Line 11 
    initLineInfo(line);
    line.line_num        = 20;
    line.addr            = 0x20A;
    line.label           = "Draw_Column_C";
    line.opcode.opcode   = 0x8;    
    line.opcode.mnemonic = "ADD";
    line.vx              = 0xB;
    line.kk              = 0x1;
    line.is_imm          = true;
    info.add(line);
    // Line 12 
    initLineInfo(line);
    line.line_num        = 21;
    line.addr            = 0x20B;
    line.symbol          = "Column";
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.ireg            = true;
    info.add(line);
    // Line 13 
    initLineInfo(line);
    line.line_num        = 22;
    line.addr            = 0x20C;
    line.opcode.opcode   = 0xB;
    line.opcode.mnemonic = "LD";
    line.ireg            = true;
    info.add(line);

    return info;
}


TEST_F(TestLexer, test_draw_asm)
{
    std::string src_filename = "data/draw.asm";
    Lexer lexer;
    lexer.setVerbose(true);
    lexer.loadFile(src_filename);

    lexer.lex();
    // Get the output and expected source info objects 
    SourceInfo lex_output = lexer.getSourceInfo();
    SourceInfo exp_output = get_draw_expected_source_info();

    // Show the expected output 
    std::cout << "\t Expected lexer output for source file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < exp_output.getNumLines(); ++idx)
        exp_output.printLine(idx);

    std::cout << std::endl;
    // Show the actual lexer output 
    std::cout << "\t Lexer output for file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
        lex_output.printLine(idx);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


