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
    line.init();
    line.line_num        = 3;
    line.addr            = 0x200;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0x1;
    line.kk              = 0x08;
    line.is_imm          = true;
    info.add(line);
    // Line 2 
    line.init();
    line.line_num        = 4;
    line.addr            = 0x201;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0x2;
    line.kk              = 0x00;
    line.is_imm          = true;
    info.add(line);
    // Line 3 
    line.init();
    line.line_num        = 7;
    line.addr            = 0x202;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0xA;
    line.kk              = 0x05;
    line.is_imm          = true;
    info.add(line);
    // Line 4 
    line.init();
    line.line_num        = 8;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    info.add(line);
    // Line 5 
    line.init();
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
    line.init();
    line.line_num        = 12;
    line.addr            = 0x205;
    line.symbol          = "Line";
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.nnn             = 0x218;
    line.reg_flags       = LEX_IREG;
    info.add(line);
    // Line 7 
    line.init();
    line.line_num        = 13;
    line.addr            = 0x206;
    line.opcode.opcode   = 0x11;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 8 
    line.init();
    line.line_num        = 14;
    line.addr            = 0x207;
    line.opcode.opcode   = 0x5;    
    line.opcode.mnemonic = "SE";
    line.vx              = 0xB;
    line.vy              = 0x1;
    info.add(line);
    // Line 9 
    line.init();
    line.line_num        = 15;
    line.addr            = 0x208;
    line.symbol          = "Draw_Column_C";
    line.opcode.opcode   = 0x3;    
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x20A;
    info.add(line);
    // Line 10 
    line.init();
    line.line_num        = 16;
    line.addr            = 0x209;
    line.symbol          = "Draw_H";
    line.opcode.opcode   = 0x3;    
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x210;
    info.add(line);
    // Line 11 
    line.init();
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
    line.init();
    line.line_num        = 21;
    line.addr            = 0x20B;
    line.symbol          = "Column";
    line.opcode.opcode   = 0x7;    
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_IREG;
    line.nnn             = 0x219;
    info.add(line);
    // Line 13 
    line.init();
    line.line_num        = 22;
    line.addr            = 0x20C;
    line.opcode.opcode   = 0x11;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 14
    line.init();
    line.line_num        = 23;
    line.addr            = 0x20D;
    line.opcode.opcode   = 0x5;
    line.opcode.mnemonic = "SE";
    line.vx              = 0xB;
    line.kk              = 0x07;
    line.is_imm          = true;
    info.add(line);
    // Line 15
    line.init();
    line.line_num        = 24;
    line.addr            = 0x20E;
    line.symbol          = "Draw_Column_C";
    line.opcode.opcode   = 0x3;
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x20A;
    info.add(line);
    // Line 16
    line.init();
    line.line_num        = 25;
    line.addr            = 0x20F;
    line.symbol          = "Draw_Line_C";
    line.opcode.opcode   = 0x3;
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x204;
    info.add(line);
    // Line 17
    line.init();
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
    line.init();
    line.line_num        = 29;
    line.addr            = 0x211;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xB;
    line.kk              = 0x01;
    line.is_imm          = true;
    info.add(line);
    // Line 19
    line.init();
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
    line.init();
    line.line_num        = 33;
    line.addr            = 0x213;
    line.symbol          = "Column";
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.nnn             = 0x219;
    line.reg_flags       = LEX_IREG;
    info.add(line);
    // Line 21
    line.init();
    line.line_num        = 34;
    line.addr            = 0x214;
    line.opcode.opcode   = 0x11;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.kk              = 0x1;
    info.add(line);
    // Line 22
    line.init();
    line.line_num        = 35;
    line.addr            = 0x215;
    line.opcode.opcode   = 0x5;
    line.opcode.mnemonic = "SE";
    line.vx              = 0xB;
    line.vy              = 0x1;
    info.add(line);
    // Line 23
    line.init();
    line.line_num        = 36;
    line.addr            = 0x216;
    line.symbol          = "Draw_Column_H";
    line.opcode.opcode   = 0x3;
    line.opcode.mnemonic = "JP";
    line.nnn             = 0x212;
    info.add(line);

    // RET
    line.init();
    line.line_num        = 38;
    line.addr            = 0x217;
    line.opcode.opcode   = 0x2;
    line.opcode.mnemonic = "RET";
    info.add(line);
    // DW 0x7000
    line.init();
    line.line_num        = 42;
    line.label           = "Line";
    line.addr            = 0x218;
    line.opcode.opcode   = 0x14;
    line.opcode.mnemonic = "DW";
    line.nnn             = 0x7000;
    line.is_label        = true;
    info.add(line);
    // DW 0x8000
    line.init();
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
    LineInfo   exp_line, lex_line;

    // Show the expected output 
    std::cout << "\t Expected lexer output for source file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < exp_output.getNumLines(); ++idx)
    {
        exp_line = exp_output.get(idx);
        std::cout << exp_line.toString() << std::endl;
    }
    std::cout << std::endl;
    // Show the actual lexer output 
    std::cout << "\t Lexer output for file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
    {
        lex_line = lex_output.get(idx);
        std::cout << lex_line.toString() << std::endl;
    }

    // Automatically compare
    ASSERT_EQ(exp_output.getNumLines(), lex_output.getNumLines());
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
    {
        lex_line = lex_output.get(idx);
        exp_line = exp_output.get(idx);
        std::cout << "Checking instruction " << std::dec << idx 
            << "/" << lex_output.getNumLines() << std::endl;
        printLineDiff(exp_line, lex_line);
        ASSERT_EQ(exp_line, lex_line);
    }
}

SourceInfo get_instr_expected_source_info(void)
{
    SourceInfo info;
    LineInfo line;

    // LD, V1, 0x04
    line.init();
    line.line_num        = 10;
    line.addr            = 0x200;
    line.label           = "IMMEDIATE_SECTION";
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0x1;
    line.kk              = 0x04;
    line.is_label        = true;
    info.add(line);
    // LD, VC, 0xAA
    line.init();
    line.line_num        = 11;
    line.addr            = 0x201;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xC;
    line.kk              = 0xAA;
    info.add(line);

    // SKP VA
    line.init();
    line.line_num        = 14;
    line.addr            = 0x202;
    line.label           = "PARSE_VX_SECTION";
    line.opcode.opcode   = 0x12;
    line.opcode.mnemonic = "SKP";
    line.vx              = 0xA;
    line.is_label        = true;
    info.add(line);
    // SKNP VB
    line.init();
    line.line_num        = 15;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x13;
    line.opcode.mnemonic = "SKNP";
    line.vx              = 0xB;
    info.add(line);

    // Special tokens 
    // LD [I] V5
    line.init();
    line.line_num        = 18;
    line.addr            = 0x204;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_IST;
    line.vy              = 0x5;
    line.label           = "SPECIAL_TOKENS";
    line.is_label        = true;
    line.is_imm          = true;
    info.add(line);
    // LD F V0
    line.init();
    line.line_num        = 19;
    line.addr            = 0x205;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_FREG;
    line.vy              = 0xF;
    info.add(line);
    // LD B, V8
    line.init();
    line.line_num        = 20;
    line.addr            = 0x206;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_BREG;
    line.vy              = 0x8;
    info.add(line);
    // LD VE, [I]
    line.init();
    line.line_num        = 21;
    line.addr            = 0x207;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_ILD;
    line.vx              = 0xE;
    info.add(line);

    // ARITHMETIC SECTION
    // LD VA, VB
    line.init();
    line.line_num        = 24;
    line.addr            = 0x208;
    line.label           = "ARITHMETIC";
    line.opcode.opcode   = LEX_LD;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xA;
    line.vy              = 0xB;
    line.is_label        = true;
    info.add(line);
    // OR V1, V2
    line.init();
    line.line_num        = 25;
    line.addr            = 0x209;
    line.opcode.opcode   = LEX_OR;
    line.opcode.mnemonic = "OR";
    line.vx              = 0x1;
    line.vy              = 0xF;
    info.add(line);
    // AND VC, VD
    line.init();
    line.line_num        = 26;
    line.addr            = 0x20A;
    line.opcode.opcode   = LEX_AND;
    line.opcode.mnemonic = "AND";
    line.vx              = 0xC;
    line.vy              = 0xD;
    info.add(line);
    // XOR V3, V4
    line.init();
    line.line_num        = 27;
    line.addr            = 0x20B;
    line.opcode.opcode   = LEX_XOR;
    line.opcode.mnemonic = "XOR";
    line.vx              = 0x3;
    line.vy              = 0x4;
    info.add(line);
    // ADD V7, V7
    line.init();
    line.line_num        = 28;
    line.addr            = 0x20C;
    line.opcode.opcode   = LEX_ADD;
    line.opcode.mnemonic = "ADD";
    line.vx              = 0x7;
    line.vy              = 0x7;
    info.add(line);
    // SUB V3, V9
    line.init();
    line.line_num        = 29;
    line.addr            = 0x20D;
    line.opcode.opcode   = LEX_SUB;
    line.opcode.mnemonic = "SUB";
    line.vx              = 0x3;
    line.vy              = 0x9;
    info.add(line);

    // SUBN V5, VA
    line.init();
    line.line_num        = 30;
    line.addr            = 0x20E;
    line.opcode.opcode   = LEX_SUBN;
    line.opcode.mnemonic = "SUBN";
    line.vx              = 0x5;
    line.vy              = 0xA;
    info.add(line);

    // Draw and random number
    // RND V3, 0x56
    line.init();
    line.line_num        = 33;
    line.addr            = 0x20F;
    line.label           = "RND_DRAW";
    line.opcode.opcode   = LEX_RND;
    line.opcode.mnemonic = "RND";
    line.vx              = 0xE;
    line.kk              = 0x56;
    line.is_label        = true;
    info.add(line);
    // DRW VF, V4
    line.init();
    line.line_num        = 34;
    line.addr            = 0x210;
    line.opcode.opcode   = LEX_DRW;
    line.opcode.mnemonic = "DRW";
    line.vx              = 0xF;
    line.vy              = 0x4;
    line.kk              = 0x2;
    info.add(line);
    // JMP_AND_CALL
    // CLS 
    line.init();
    line.line_num        = 37;
    line.addr            = 0x211;
    line.label           = "JMP_AND_CALL";
    line.opcode.opcode   = LEX_CLS;
    line.opcode.mnemonic = "CLS";
    line.is_label        = true;
    info.add(line);
    // RET 
    line.init();
    line.line_num        = 38;
    line.addr            = 0x212;
    line.opcode.opcode   = LEX_RET;
    line.opcode.mnemonic = "RET";
    info.add(line);
    // SYS 0x040
    line.init();
    line.line_num        = 39;
    line.addr            = 0x213;
    line.opcode.opcode   = LEX_SYS;
    line.opcode.mnemonic = "SYS";
    line.nnn             = 0x040;
    info.add(line);
    // SYS 0xA1A
    line.init();
    line.line_num        = 40;
    line.addr            = 0x214;
    line.opcode.opcode   = LEX_SYS;
    line.opcode.mnemonic = "SYS";
    line.nnn             = 0xA1A;
    info.add(line);
    // CALL 0x443
    line.init();
    line.line_num        = 41;
    line.addr            = 0x215;
    line.opcode.opcode   = LEX_CALL;
    line.opcode.mnemonic = "CALL";
    line.nnn             = 0x443;
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
    LineInfo   exp_line, lex_line;

    // Show the expected output 
    std::cout << "\t Expected lexer output for source file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < exp_output.getNumLines(); ++idx)
    {
        exp_line = exp_output.get(idx);
        std::cout << exp_line.toString() << std::endl;
    }

    std::cout << std::endl;
    // Show the actual lexer output 
    std::cout << "\t Lexer output for file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
    {
        lex_line = lex_output.get(idx);
        std::cout << lex_line.toString() << std::endl;
    }
    
    std::cout << lex_output.dumpErrors() << std::endl;
    ASSERT_EQ(0, lex_output.getNumError());

    // Automatically compare
    ASSERT_EQ(exp_output.getNumLines(), lex_output.getNumLines());
    for(unsigned int idx = 0; idx < lex_output.getNumLines(); ++idx)
    {
        lex_line = lex_output.get(idx);
        exp_line = exp_output.get(idx);
        std::cout << "Checking line " << std::dec << idx << "/" << lex_output.getNumLines() << std::endl;
        printLineDiff(exp_line, lex_line);
        ASSERT_EQ(exp_line, lex_line);
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


