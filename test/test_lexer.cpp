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
    line.is_imm          = true;
    info.add(line);
    // Line 2 
    initLineInfo(line);
    line.line_num        = 4;
    line.addr            = 0x201;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0x2;
    line.kk              = 0x00;
    line.is_imm          = true;
    info.add(line);
    // Line 3 
    initLineInfo(line);
    line.line_num        = 7;
    line.addr            = 0x202;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0xA;
    line.kk              = 0x05;
    line.is_imm          = true;
    info.add(line);
    // Line 4 
    initLineInfo(line);
    line.line_num        = 8;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    info.add(line);
    // Line 5 
    initLineInfo(line);
    line.line_num        = 11;
    line.addr            = 0x204;
    line.label           = "Draw_Line_C";
    line.opcode.opcode   = 0x8;    
    line.opcode.mnemonic = "ADD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    line.is_label        = true;
    info.add(line);
    // Line 6 
    initLineInfo(line);
    line.line_num        = 12;
    line.addr            = 0x205;
    line.symbol          = "Line";
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.nnn             = 0x218;
    line.ireg            = true;
    info.add(line);
    // Line 7 
    initLineInfo(line);
    line.line_num        = 13;
    line.addr            = 0x206;
    line.opcode.opcode   = 0x11;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 8 
    initLineInfo(line);
    line.line_num        = 14;
    line.addr            = 0x207;
    line.opcode.opcode   = 0x5;    
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
    line.nnn             = 0x20A;
    info.add(line);
    // Line 10 
    initLineInfo(line);
    line.line_num        = 16;
    line.addr            = 0x209;
    line.symbol          = "Draw_H";
    line.opcode.opcode   = 0x3;    
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x210;
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
    line.is_label        = true;
    info.add(line);
    // Line 12 
    initLineInfo(line);
    line.line_num        = 21;
    line.addr            = 0x20B;
    line.symbol          = "Column";
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.ireg            = true;
    line.nnn             = 0x219;
    info.add(line);
    // Line 13 
    initLineInfo(line);
    line.line_num        = 22;
    line.addr            = 0x20C;
    line.opcode.opcode   = 0x11;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 14
    initLineInfo(line);
    line.line_num        = 23;
    line.addr            = 0x20D;
    line.opcode.opcode   = 0x5;
    line.opcode.mnemonic = "SE";
    line.vx              = 0xB;
    line.kk              = 0x07;
    line.is_imm          = true;
    info.add(line);
    // Line 15
    initLineInfo(line);
    line.line_num        = 24;
    line.addr            = 0x20E;
    line.symbol          = "Draw_Column_C";
    line.opcode.opcode   = 0x3;
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x20A;
    info.add(line);
    // Line 16
    initLineInfo(line);
    line.line_num        = 25;
    line.addr            = 0x20F;
    line.symbol          = "Draw_Line_C";
    line.opcode.opcode   = 0x3;
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x204;
    info.add(line);
    // Line 17
    initLineInfo(line);
    line.line_num        = 28;
    line.addr            = 0x210;
    line.label           = "Draw_H";
    line.opcode.opcode   = 0x8;
    line.opcode.mnemonic = "ADD";
    line.vx              = 0xA;
    line.kk              = 0x05;
    line.is_imm          = true;
    line.is_label        = true;
    info.add(line);
    // Line 18
    initLineInfo(line);
    line.line_num        = 29;
    line.addr            = 0x211;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    info.add(line);
    // Line 19
    initLineInfo(line);
    line.line_num        = 32;
    line.addr            = 0x212;
    line.label           = "Draw_Column_H";
    line.opcode.opcode   = 0x8;
    line.opcode.mnemonic = "ADD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    line.is_label        = true;
    info.add(line);
    // Line 20
    initLineInfo(line);
    line.line_num        = 33;
    line.addr            = 0x213;
    line.symbol          = "Column";
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.nnn             = 0x219;
    line.ireg            = true;
    info.add(line);
    // Line 21
    initLineInfo(line);
    line.line_num        = 34;
    line.addr            = 0x214;
    line.opcode.opcode   = 0x11;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 22
    initLineInfo(line);
    line.line_num        = 35;
    line.addr            = 0x215;
    line.opcode.opcode   = 0x5;
    line.opcode.mnemonic = "SE";
    line.vx              = 0xB;
    line.vy              = 0x1;
    info.add(line);
    // Line 23
    initLineInfo(line);
    line.line_num        = 36;
    line.addr            = 0x216;
    line.symbol          = "Draw_Column_H";
    line.opcode.opcode   = 0x3;
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x212;
    info.add(line);

    // RET
    initLineInfo(line);
    line.line_num        = 38;
    line.addr            = 0x217;
    line.opcode.opcode   = 0x2;
    line.opcode.mnemonic = "RET";
    info.add(line);
    // DW 0x7000
    initLineInfo(line);
    line.line_num        = 42;
    line.label           = "Line";
    line.addr            = 0x218;
    line.opcode.opcode   = 0x14;
    line.opcode.mnemonic = "DW";
    line.nnn             = 0x7000;
    line.is_label        = true;
    info.add(line);
    // DW 0x8000
    initLineInfo(line);
    line.line_num        = 45;
    line.label           = "Column";
    line.addr            = 0x219;
    line.opcode.opcode   = 0x14;
    line.opcode.mnemonic = "DW";
    line.nnn             = 0x8000;
    line.is_label        = true;
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

    // Automatically compare
    ASSERT_EQ(exp_output.getNumLines(), lex_output.getNumLines());
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
    {
        LineInfo lex_line = lex_output.get(idx);
        LineInfo exp_line = exp_output.get(idx);
        std::cout << "Checking instruction " << std::dec << idx 
            << "/" << lex_output.getNumLines() << std::endl;
        printLineDiff(exp_line, lex_line);
        bool diff = compLineInfo(exp_line, lex_line);
        ASSERT_EQ(true, diff);
    }
}

SourceInfo get_instr_expected_source_info(void)
{
    SourceInfo info;
    LineInfo line;

    // LD, V1, 0x04
    initLineInfo(line);
    line.line_num        = 10;
    line.addr            = 0x200;
    line.label           = "IMMEDIATE_SECTION";
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0x1;
    line.kk              = 0x04;
    info.add(line);
    // LD, VC, 0xAA
    initLineInfo(line);
    line.line_num        = 11;
    line.addr            = 0x201;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xC;
    line.kk              = 0xAA;
    info.add(line);

    // SKP VA
    initLineInfo(line);
    line.line_num        = 14;
    line.addr            = 0x202;
    line.label           = "PARSE_VX_SECTION";
    line.opcode.opcode   = 0x12;
    line.opcode.mnemonic = "SKP";
    line.vx              = 0xA;
    info.add(line);
    // SKNP VB
    initLineInfo(line);
    line.line_num        = 15;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x13;
    line.opcode.mnemonic = "SKNP";
    line.vx              = 0xB;
    info.add(line);

    return info;
}

TEST_F(TestLexer, test_instr_asm)
{
    std::string src_filename = "data/instr.asm";
    Lexer lexer;
    lexer.setVerbose(true);
    lexer.loadFile(src_filename);

    lexer.lex();
    // Get the output and expected source info objects 
    SourceInfo lex_output = lexer.getSourceInfo();
    SourceInfo exp_output = get_instr_expected_source_info();

    // Show the expected output 
    std::cout << "\t Expected lexer output for source file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < exp_output.getNumLines(); ++idx)
        exp_output.printLine(idx);

    std::cout << std::endl;
    // Show the actual lexer output 
    std::cout << "\t Lexer output for file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
        lex_output.printLine(idx);

    // Automatically compare
    ASSERT_EQ(exp_output.getNumLines(), lex_output.getNumLines());
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
    {
        LineInfo lex_line = lex_output.get(idx);
        LineInfo exp_line = exp_output.get(idx);
        bool diff = compLineInfo(exp_line, lex_line);
        ASSERT_EQ(false, diff);
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


