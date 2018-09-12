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
    prog.add(instr);
    // LD VB, 0x01
    instr.ins = 0x6B01;
    instr.adr = 0x203;
    prog.add(instr);
    // ADD Vb, 0x01
    instr.ins = 0x7B01;
    instr.adr = 0x204;
    prog.add(instr);
    // LD I, Line <- (0x218)
    instr.ins = 0xA218;
    instr.adr = 0x205;
    prog.add(instr);
    // DRW Va, Vb, 0x01
    instr.ins = 0xDAB1;
    instr.adr = 0x206;
    prog.add(instr);
    // SE Vb, V1,
    instr.ins = 0x5B10;
    instr.adr = 0x207;
    prog.add(instr);
    // JP Draw_Column_C <- (0x20A)
    instr.ins = 0x120A;
    instr.adr = 0x208;
    prog.add(instr);
    // JP Draw_H <- (0x21)
    instr.ins = 0x1210;
    instr.adr = 0x209;
    prog.add(instr);

// Draw_Column_C
    // ADD Bv, 0x01
    instr.ins = 0x7b01;
    instr.adr = 0x20A;
    prog.add(instr);
    // LD I, Column 
    instr.ins = 0xA21A;
    instr.adr = 0x20B;
    prog.add(instr);
    // DRW Va. Vb, 0x01
    instr.ins = 0xDAB1;
    instr.adr = 0x20C;
    prog.add(instr);
    // SE Vb, 0x07
    instr.ins = 0x3B07;
    instr.adr = 0x20D;
    prog.add(instr);
    // JP Draw_Column_C <- (0x20A)
    instr.ins = 0x120A;
    instr.adr = 0x20E;
    prog.add(instr);
    // JP Draw_Line_C <- (0x204)
    instr.ins = 0x1204;
    instr.adr = 0x20F;
    prog.add(instr);


// Draw_H
    // ADD Va, 0x05
    instr.ins = 0x7A05;
    instr.adr = 0x210;
    prog.add(instr);
    // LD Vb, 0x01
    instr.ins = 0x6B01;
    instr.adr = 0x211;
    prog.add(instr);

// Draw_Column_H
    // ADD, VB, 0x01
    instr.ins = 0x7B01;
    instr.adr = 0x212;
    prog.add(instr);
    // LD, I, Column <- (0x21A)
    instr.ins = 0xA21A;
    instr.adr = 0x213;
    prog.add(instr);
    // DRW Va, Vb, 0x1
    instr.ins = 0xDAB1;
    instr.adr = 0x214;
    prog.add(instr);
    // SE Vb, V1
    instr.ins = 0x5B10;
    instr.adr = 0x215;
    prog.add(instr);
    // JP Draw_Column_H <- (0x212)
    instr.ins = 0x1212;
    instr.adr = 0x216;
    prog.add(instr);
    // RET
    instr.ins = 0x00EE;
    instr.adr = 0x217;
    prog.add(instr);
    // DW 0x7000
    // DW 0x8000

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
    // Show the symbol table contents
    std::cout << "\t Dumping lexer symbol table contents...." << std::endl;

    SymbolTable sym_table = lexer.getSymTable();
    for(unsigned int s = 0; s < sym_table.getNumSyms(); ++s)
    {
        Symbol sym = sym_table.get(s);
        std::cout << "[" << std::left << std::setw(16) << std::setfill(' ') << sym.label << "]  ";
        std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << sym.addr << std::endl;
    }
    std::cout << std::endl;


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
        std::cout << "<" << std::right << std::setw(3) << i << "> " << std::right << as_program.getStr(i);
    }

    Program expected_program = get_draw_expected_program();
    for(unsigned int i = 0; i < as_program.numInstr(); ++i)
    {
        Instr as_instr = as_program.get(i);
        Instr ex_instr = expected_program.get(i);
        std::cout << "Checking instruction " << i << "/" << expected_program.numInstr() << "\n";
        ASSERT_EQ(ex_instr.ins, as_instr.ins);
        ASSERT_EQ(ex_instr.adr, as_instr.adr);
    }
    std::cout << std::endl << "done " << std::endl;
}


Program get_instr_expected_program(void)
{
    Program prog;
    Instr instr;

    // LD, V1, 0x04
    instr.ins = 0x6104;
    instr.adr = 0x200;
    prog.add(instr);
    // LD VC, 0xAA
    instr.ins = 0x6CAA;
    instr.adr = 0x201;
    prog.add(instr);
    // SKP VA
    instr.ins = 0xEA9E;
    instr.adr = 0x202;
    prog.add(instr);
    // SNKP VB
    instr.ins = 0xEBA1;
    instr.adr = 0x203;
    prog.add(instr);

    return prog;
}

TEST_F(TestAssembler, test_asm_instr)
{
    SourceInfo lex_output;
    std::string filename = "data/instr.asm";

    Lexer lexer;
    lexer.setVerbose(true);
    lexer.loadFile(filename);
    std::cout << "\t Lexing source file " << filename << std::endl;
    lexer.lex();
    lex_output = lexer.getSourceInfo();
    std::cout << "Lexer produced " << lex_output.getNumLines() << " lines of output" << std::endl;
    // Show the symbol table contents
    std::cout << "\t Dumping lexer symbol table contents...." << std::endl;

    SymbolTable sym_table = lexer.getSymTable();
    for(unsigned int s = 0; s < sym_table.getNumSyms(); ++s)
    {
        Symbol sym = sym_table.get(s);
        std::cout << "[" << std::left << std::setw(16) << std::setfill(' ') << sym.label << "]  ";
        std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << sym.addr << std::endl;
    }
    std::cout << std::endl;


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
        std::cout << "<" << std::right << std::setw(3) << i << "> " << std::right << as_program.getStr(i);
    }

    Program expected_program = get_instr_expected_program();
    for(unsigned int i = 0; i < as_program.numInstr(); ++i)
    {
        Instr as_instr = as_program.get(i);
        Instr ex_instr = expected_program.get(i);
        std::cout << "Checking instruction " << i << "/" << expected_program.numInstr() << "\n";
        ASSERT_EQ(ex_instr.ins, as_instr.ins);
        ASSERT_EQ(ex_instr.adr, as_instr.adr);
    }
    std::cout << std::endl << "done " << std::endl;

}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

