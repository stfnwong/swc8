/* 
 * TEST_PROGRAM
 * Program object unit test 
 *
 * Stefan Wong 2018
 */

#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "lexer.hpp"
#include "assembler.hpp"
#include "program.hpp"

class TestProgram : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(TestProgram, test_load_program)
{
    int status;
    Lexer lexer;
    Assembler assembler;
    Program program;
    std::string src_filename = "data/draw.asm";
    std::string program_filename = "data/draw.obj";

    lexer.setVerbose(false);
    lexer.loadFile(src_filename);
    
    std::cout << "\t Lexing file " << src_filename << std::endl;
    lexer.lex();
    assembler.setVerbose(false);
    assembler.loadSource(lexer.getSourceInfo());
    std::cout << "\t Assembling file " << src_filename << std::endl;
    assembler.assemble();

    program = assembler.getProgram();    
    std::cout << "\t Dumping program output for file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < program.numInstr(); ++idx)
    {
        Instr instr = program.get(idx);
        std::cout << "<" << std::right << std::hex << std::setw(4) << std::setfill('0') << idx << "> ";
        std::cout << "[0x" << std::right << std::hex << std::setw(4) << instr.adr << "]   ";
        std::cout << std::right << std::hex << std::setw(4) << std::setfill('0') << instr.ins << std::endl;
    }

    std::cout << "\t Writing program to file " << program_filename << std::endl;
    status = program.save(program_filename);
    ASSERT_EQ(0, status);

    std::cout << "\t Reading output file " << program_filename << " into new program object" << std::endl;
    Program read_prog;
    status = read_prog.load(program_filename);
    ASSERT_EQ(0, status);

    std::cout << "Comparing output program with program in file " << program_filename << std::endl;
    ASSERT_EQ(program.numInstr(), read_prog.numInstr());
    std::cout << "\t Dumping program output for file " << program_filename << std::endl;
    for(unsigned int idx = 0; idx < read_prog.numInstr(); ++idx)
    {
        Instr instr = read_prog.get(idx);
        std::cout << "<" << std::right << std::hex << std::setw(4) << std::setfill('0') << idx << "> ";
        std::cout << "[0x" << std::right << std::hex << std::setw(4) << instr.adr << "]   ";
        std::cout << std::right << std::hex << std::setw(4) << std::setfill('0') << instr.ins << std::endl;
    }

    for(unsigned int idx = 0; idx < program.numInstr(); ++idx)
    {
        std::cout << "Comparing instruction " << idx << "/" << program.numInstr() << std::endl;
        Instr prog_instr = program.get(idx);
        Instr file_instr = read_prog.get(idx);
        ASSERT_EQ(prog_instr.adr, file_instr.adr);
        ASSERT_EQ(prog_instr.ins, file_instr.ins);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

