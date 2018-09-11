/* TEST_ASSEMBLER 
 * Tests for Chip8 psuedo-assembler 
 *
 * Stefan Wong 2018
 */

#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "assembler.hpp"
#include "lexer.hpp"
#include "source.hpp"

class TestAssembler : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

//TEST_F(TestAssembler, test_init)
//{
//    Assembler as;
//
//}

Program get_draw_expected_program(void)
{
    Program prog;
    Instr instr;

    // LD V1, 0x08
    instr.ins = 0x6108;
    instr.adr = 0x200;
    prog.add(instr);
    // LD, V2, 0x00
    instr.ins = 0x6200;
    instr.adr = 0x201;
    prog.add(instr);
    // LD VA, 0x05
    instr.ins = 0x6A05;
    instr.adr = 0x202;
    // LD VB, 0x01
    instr.ins = 0x6B01;
    instr.adr = 0x203;
    // ADD Vb, 0x01

    return prog;
}

TEST_F(TestAssembler, test_asm_draw)
{
    SourceInfo lex_output;
    std::string filename = "data/draw.asm";

    Lexer lexer;
    lexer.setVerbose(true);
    lexer.loadFile(filename);
    std::cout << "\t Lexing source file " << filename << std::endl;
    lexer.lex();
    lex_output = lexer.getSourceInfo();
    std::cout << "Lexer produced " << lex_output.getNumLines() << " lines of output" << std::endl;
    //ASSERT_GT(0, lex_output.getNumLines());

    Assembler as;
    as.setVerbose(true);
    std::cout <<"\t Loading lexer output into assembler" << std::endl;
    as.loadSource(lex_output);
    std::cout << "\t Assembling file " << filename << std::endl;
    as.assemble();

    Program as_program = as.getProgram();
    std::cout << "Assembled " << as_program.numInstr() << " instructions in program " << filename << std::endl;
    std::cout << "\t Dumping assembly output...." << std::endl;
    for(unsigned int i = 0; i < as_program.numInstr(); ++i)
    {
        std::cout << as_program.getStr(i);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

