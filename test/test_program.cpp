/* 
 * TEST_PROGRAM
 * Program object unit test 
 *
 * Stefan Wong 2018
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <string>
#include <vector>
#include "lexer.hpp"
#include "assembler.hpp"
#include "program.hpp"


TEST_CASE("test_load_program", "[classic]")
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
    REQUIRE(0 == status);

    std::cout << "\t Reading output file " << program_filename << " into new program object" << std::endl;
    Program read_prog;
    status = read_prog.load(program_filename);
    REQUIRE(0 == status);

    std::cout << "Comparing output program with program in file " << program_filename << std::endl;
    REQUIRE(program.numInstr() == read_prog.numInstr());
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
        REQUIRE(prog_instr.adr == file_instr.adr);
        REQUIRE(prog_instr.ins == file_instr.ins);
    }
}

TEST_CASE("test_read_write_object", "[classic]")
{
    int status;
    Lexer lexer;
    Assembler assembler;
    Program program;
    std::string src_filename = "data/instr.asm";
    std::string obj_filename = "data/instr.obj";

    lexer.setVerbose(false);
    lexer.loadFile(src_filename);
    
    std::cout << "\t Lexing file " << src_filename << std::endl;
    lexer.lex();
    assembler.setVerbose(false);
    assembler.loadSource(lexer.getSourceInfo());
    std::cout << "\t Assembling file " << src_filename << std::endl;
    assembler.assemble();

    program = assembler.getProgram();    
    program.setVerbose(true);
    std::cout << "\t Dumping program output for file " << src_filename << std::endl;
    for(unsigned int idx = 0; idx < program.numInstr(); ++idx)
    {
        Instr instr = program.get(idx);
        std::cout << "<" << std::right << std::hex << std::setw(4) << std::setfill('0') << idx << "> ";
        std::cout << "[0x" << std::right << std::hex << std::setw(4) << instr.adr << "]   ";
        std::cout << std::right << std::hex << std::setw(4) << std::setfill('0') << instr.ins << std::endl;
    }

    // Write the program object file to disk 
    std::cout << "\t Writing program object to file " << obj_filename << std::endl;
    status = program.writeObj(obj_filename);
    REQUIRE(0 == status);
    // Now read into a new program
    std::cout << "\t Reading object file (" << obj_filename << ") into new Program" << std::endl;
    Program obj_prog;
    obj_prog.setVerbose(true);
    status = obj_prog.readObj(obj_filename);
    REQUIRE(0 == status);

    std::cout << "\t Dumping object file output from file " << obj_filename << std::endl;
    for(unsigned int idx = 0; idx < obj_prog.numInstr(); ++idx)
    {
        Instr instr = obj_prog.get(idx);
        std::cout << "<" << std::right << std::hex << std::setw(4) << std::setfill('0') << idx << "> ";
        std::cout << "[0x" << std::right << std::hex << std::setw(4) << instr.adr << "]   ";
        std::cout << std::right << std::hex << std::setw(4) << std::setfill('0') << instr.ins << std::endl;
    }

    std::cout << "Checking program outputs..." << std::endl;
    for(unsigned int idx = 0; idx < program.numInstr(); ++idx)
    {
        std::cout << "Checking instruction " << idx << "/" << program.numInstr() << std::endl;
        Instr prog_instr = program.get(idx);
        Instr obj_instr  = obj_prog.get(idx);
        REQUIRE(prog_instr.adr == obj_instr.adr);
        REQUIRE(prog_instr.ins == obj_instr.ins);
    }
}
