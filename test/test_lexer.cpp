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


TEST_F(TestLexer, test_init)
{
    Lexer lex;
}

// Generate expected source info for draw.asm
SourceInfo get_draw_expected_source_info(void)
{
    SourceInfo info;
    LineInfo line;

    // Line 1 
    initLineInfo(line);
    line.line_num        = 1;
    line.addr            = 0x200;
    line.opcode.opcode   = 0x8000;    // LD
    line.opcode.mnemonic = "LD";
    line.arg1            = 0x1;
    line.arg2            = 0x08;

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

}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


