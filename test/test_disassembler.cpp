/* TEST_DISASSEMBLER 
 * Tests for Chip8 disassembler
 *
 * Stefan Wong 2018
 */

#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "disassembler.hpp"
#include "source.hpp"
// Since there is also an assembly pass, we need the assembler
#include "lexer.hpp"
#include "assembler.hpp"
#include "program.hpp"

class TestDisassembler : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

void assemble_source(const std::string& infile, const std::string& outfile)
{
    Lexer lex;
    lex.loadFile(infile);
    lex.lex();

    Assembler assem;
    assem.loadSource(lex.getSourceInfo());
    assem.assemble();

    Program prog_out = assem.getProgram();
    prog_out.save(outfile);
}

SourceInfo get_instr_expected_source_info(void)
{
    SourceInfo info;
    LineInfo line;

    // Note that we can't know about labels just from the object 
    // source. Will need a seperate symbol file for that.
    // LD, V1, 0x04
    initLineInfo(line);
    line.line_num        = 10;
    line.addr            = 0x200;
    //line.label           = "IMMEDIATE_SECTION";
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0x1;
    line.kk              = 0x04;
    //line.is_label        = true;
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
    //line.label           = "PARSE_VX_SECTION";
    line.opcode.opcode   = 0x12;
    line.opcode.mnemonic = "SKP";
    line.vx              = 0xA;
    //line.is_label        = true;
    info.add(line);
    // SKNP VB
    initLineInfo(line);
    line.line_num        = 15;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x13;
    line.opcode.mnemonic = "SKNP";
    line.vx              = 0xB;
    info.add(line);

    // Special tokens 
    // LD [I] V5
    initLineInfo(line);
    line.line_num        = 18;
    line.addr            = 0x204;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_IST;
    line.vy              = 0x5;
    //line.label           = "SPECIAL_TOKENS";
    //line.is_label        = true;
    line.is_imm          = true;
    info.add(line);
    // LD F V0
    initLineInfo(line);
    line.line_num        = 19;
    line.addr            = 0x205;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_FREG;
    line.vy              = 0xF;
    info.add(line);
    // LD B, V8
    initLineInfo(line);
    line.line_num        = 20;
    line.addr            = 0x206;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_BREG;
    line.vy              = 0x8;
    info.add(line);
    // LD VE, [I]
    initLineInfo(line);
    line.line_num        = 21;
    line.addr            = 0x207;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_ILD;
    line.vx              = 0xE;
    info.add(line);

    return info;
}

TEST_F(TestDisassembler, test_dis_instr)
{
    int status;
    std::string src_file = "data/instr.asm";
    std::string obj_file = "data/instr.obj";
    Disassembler dis;
    dis.setVerbose(true);

    // Assemble the output first 
    assemble_source(src_file, obj_file);

    std::cout << "\t Loading data from file " << obj_file << std::endl;
    status = dis.load(obj_file);
    ASSERT_EQ(0, status);

    dis.disassemble();
    SourceInfo dis_out = dis.getSourceInfo();
    SourceInfo exp_out = get_instr_expected_source_info();
    std::cout << dis_out.getNumLines() << " lines in source output" << std::endl;

    // Dump the disassembled source 
    std::cout << "\t Dumping disassembled source..." << std::endl;
    for(unsigned int idx = 0; idx < dis_out.getNumLines(); ++idx)
    {
        std::cout << dis_out.getStr(idx);
    }

    // Check actual contents 
    std::cout << "\t Checking disassembled output..." << std::endl;
    bool diff;
    for(unsigned int idx = 0; idx < dis_out.getNumLines(); ++idx)
    {
        LineInfo exp_line = exp_out.get(idx);
        LineInfo dis_line = dis_out.get(idx);
        printLineDiff(exp_line, dis_line);
        std::cout << "Checking line " << idx << "/" << dis_out.getNumLines() << std::endl;
        diff = compLineInfo(exp_line, dis_line);
        ASSERT_EQ(true, diff);
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
