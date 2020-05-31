/* TEST_ASSEMBLER 
 * Tests for Chip8 psuedo-assembler 
 *
 * Stefan Wong 2018
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <string>
#include <vector>
#include "assembler.hpp"
#include "lexer.hpp"
#include "source.hpp"


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
    instr.ins = 0xA219;
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
    // LD, I, Column <- (0x219)
    instr.ins = 0xA219;
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
    instr.ins = 0x7000;
    instr.adr = 0x218;
    prog.add(instr);
    // DW 0x8000
    instr.ins = 0x8000;
    instr.adr = 0x219;
    prog.add(instr);

    return prog;
}


TEST_CASE("test_asm_draw", "[classic]")
{
    SourceInfo lex_output;
    std::string filename = "data/draw.asm";

    Lexer lexer;
    //lexer.setVerbose(true);
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

    Program expected_program = get_draw_expected_program();
    for(unsigned int i = 0; i < as_program.numInstr(); ++i)
    {
        Instr as_instr = as_program.get(i);
        Instr ex_instr = expected_program.get(i);
        REQUIRE(ex_instr == as_instr);
    }
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
    // LD [I], V5
    instr.ins = 0xF555;
    instr.adr = 0x204;
    prog.add(instr);
    // LD F, VF
    instr.ins = 0xFF29;
    instr.adr = 0x205;
    prog.add(instr);
    // LD B, B8
    instr.ins = 0xF833;
    instr.adr = 0x206;
    prog.add(instr);
    // LD VE, [I]
    instr.ins = 0xFE65;
    instr.adr = 0x207;
    prog.add(instr);
    // Arithmetic 
    // LD VA, VB
    instr.ins = 0x8AB0;
    instr.adr = 0x208;
    prog.add(instr);
    // OR V1, VF
    instr.ins = 0x81F1;
    instr.adr = 0x209;
    prog.add(instr);
    // AND VC, VD 
    instr.ins = 0x8CD2;
    instr.adr = 0x20A;
    prog.add(instr);
    // XOR V3, V4
    instr.ins = 0x8343;
    instr.adr = 0x20B;
    prog.add(instr);
    // ADD V7, V7
    instr.ins = 0x8774;
    instr.adr = 0x20C;
    prog.add(instr);
    // SUB V3, V9
    instr.ins = 0x8395;
    instr.adr = 0x20D;
    prog.add(instr);
    // SUBN V5, VA
    instr.ins = 0x85A7;
    instr.adr = 0x20E;
    prog.add(instr);

    // RND and DRAW
    // RND VE, 0x56
    instr.ins = 0xCE56;
    instr.adr = 0x20F;
    prog.add(instr);
    // DRW VF, V4. 0x2
    instr.ins = 0xDF42;
    instr.adr = 0x210;
    prog.add(instr);

    //JMP_AND_CALL
    //CLS
    instr.ins = 0x00E0;
    instr.adr = 0x211;
    prog.add(instr);
    // RET 
    instr.ins = 0x00EE;
    instr.adr = 0x212;
    prog.add(instr);
    // SYS 0x040
    instr.ins = 0x0040;
    instr.adr = 0x213;
    prog.add(instr);
    // SYS 0xA1A
    instr.ins = 0x0A1A;
    instr.adr = 0x214;
    prog.add(instr);
    // CALL 0x443
    instr.ins = 0x2443;
    instr.adr = 0x215;
    prog.add(instr);

    return prog;
}


TEST_CASE("test_asm_instr", "[classic]")
{
    SourceInfo lex_output;
    std::string filename = "data/instr.asm";

    Lexer lexer;
    //lexer.setVerbose(true);
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

    Program expected_program = get_instr_expected_program();
    for(unsigned int i = 0; i < as_program.numInstr(); ++i)
    {
        Instr as_instr = as_program.get(i);
        Instr ex_instr = expected_program.get(i);
        REQUIRE(ex_instr == as_instr);
    }
}
