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
    line.line_num        = 1;
    line.addr            = 0x200;
    //line.label           = "IMMEDIATE_SECTION";
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0x1;
    line.kk              = 0x04;
    //line.is_label        = true;
    info.add(line);
    // LD, VC, 0xAA
    line.init();
    line.line_num        = 2;
    line.addr            = 0x201;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xC;
    line.kk              = 0xAA;
    info.add(line);

    // SKP VA
    line.init();
    line.line_num        = 3;
    line.addr            = 0x202;
    //line.label           = "PARSE_VX_SECTION";
    line.opcode.opcode   = 0x12;
    line.opcode.mnemonic = "SKP";
    line.vx              = 0xA;
    //line.is_label        = true;
    info.add(line);
    // SKNP VB
    line.init();
    line.line_num        = 4;
    line.addr            = 0x203;
    line.opcode.opcode   = 0x13;
    line.opcode.mnemonic = "SKNP";
    line.vx              = 0xB;
    info.add(line);

    // Special tokens 
    // LD [I] V5
    line.init();
    line.line_num        = 5;
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
    line.init();
    line.line_num        = 6;
    line.addr            = 0x205;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_FREG;
    line.vy              = 0xF;
    info.add(line);
    // LD B, V8
    line.init();
    line.line_num        = 7;
    line.addr            = 0x206;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_BREG;
    line.vy              = 0x8;
    info.add(line);
    // LD VE, [I]
    line.init();
    line.line_num        = 8;
    line.addr            = 0x207;
    line.opcode.opcode   = 0x7;
    line.opcode.mnemonic = "LD";
    line.reg_flags       = LEX_ILD;
    line.vx              = 0xE;
    info.add(line);

    // ARITHMETIC SECTION
    // LD VA, VB
    line.init();
    line.line_num        = 9;
    line.addr            = 0x208;
    line.opcode.opcode   = LEX_LD;
    line.opcode.mnemonic = "LD";
    line.vx              = 0xA;
    line.vy              = 0xB;
    info.add(line);
    // OR V1, V2
    line.init();
    line.line_num        = 10;
    line.addr            = 0x209;
    line.opcode.opcode   = LEX_OR;
    line.opcode.mnemonic = "OR";
    line.vx              = 0x1;
    line.vy              = 0xF;
    info.add(line);
    // AND VC, VD
    line.init();
    line.line_num        = 11;
    line.addr            = 0x20A;
    line.opcode.opcode   = LEX_AND;
    line.opcode.mnemonic = "AND";
    line.vx              = 0xC;
    line.vy              = 0xD;
    info.add(line);
    // XOR V3, V4
    line.init();
    line.line_num        = 12;
    line.addr            = 0x20B;
    line.opcode.opcode   = LEX_XOR;
    line.opcode.mnemonic = "XOR";
    line.vx              = 0x3;
    line.vy              = 0x4;
    info.add(line);
    // ADD V7, V7
    line.init();
    line.line_num        = 13;
    line.addr            = 0x20C;
    line.opcode.opcode   = LEX_ADD;
    line.opcode.mnemonic = "ADD";
    line.vx              = 0x7;
    line.vy              = 0x7;
    info.add(line);
    // SUB V3, V9
    line.init();
    line.line_num        = 14;
    line.addr            = 0x20D;
    line.opcode.opcode   = LEX_SUB;
    line.opcode.mnemonic = "SUB";
    line.vx              = 0x3;
    line.vy              = 0x9;
    info.add(line);

    // SUBN V5, VA
    line.init();
    line.line_num        = 15;
    line.addr            = 0x20E;
    line.opcode.opcode   = LEX_SUBN;
    line.opcode.mnemonic = "SUBN";
    line.vx              = 0x5;
    line.vy              = 0xA;
    info.add(line);

    // RND VE, 0x56
    line.init();
    line.line_num        = 16;
    line.addr            = 0x20F;
    line.opcode.opcode   = LEX_RND;
    line.opcode.mnemonic = "RND";
    line.vx              = 0xE;
    line.kk              = 0x56;
    info.add(line);
    // DRW VF, V4
    line.init();
    line.line_num        = 17;
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
    line.line_num        = 18;
    line.addr            = 0x211;
    line.opcode.opcode   = LEX_CLS;
    line.opcode.mnemonic = "CLS";
    info.add(line);
    // RET 
    line.init();
    line.line_num        = 19;
    line.addr            = 0x212;
    line.opcode.opcode   = LEX_RET;
    line.opcode.mnemonic = "RET";
    info.add(line);
    // SYS 0x040
    line.init();
    line.line_num        = 20;
    line.addr            = 0x213;
    line.opcode.opcode   = LEX_SYS;
    line.opcode.mnemonic = "SYS";
    line.nnn             = 0x040;
    info.add(line);
    // SYS 0xA1A
    line.init();
    line.line_num        = 21;
    line.addr            = 0x214;
    line.opcode.opcode   = LEX_SYS;
    line.opcode.mnemonic = "SYS";
    line.nnn             = 0xA1A;
    info.add(line);
    // CALL 0x443
    line.init();
    line.line_num        = 22;
    line.addr            = 0x215;
    line.opcode.opcode   = LEX_CALL;
    line.opcode.mnemonic = "CALL";
    line.nnn             = 0x443;
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

    // Dump the object file from disk 
    std::cout << "\t Dumping contents of file " << obj_file << std::endl;
    Program obj_prog = dis.getProgram();
    Instr instr;
    for(unsigned int idx = 0; idx < obj_prog.numInstr(); ++idx)
    {
        instr = obj_prog.get(idx);
        std::cout << "[" << std::right << std::hex << std::setw(4) << std::setfill('0') << instr.adr << "]  ";
        std::cout << "$" << std::right << std::hex << std::setw(4) << std::setfill('0') << instr.ins << std::endl;
    }

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
    for(unsigned int idx = 0; idx < dis_out.getNumLines(); ++idx)
    {
        LineInfo exp_line = exp_out.get(idx);
        LineInfo dis_line = dis_out.get(idx);
        printLineDiff(exp_line, dis_line);
        std::cout << "Checking line " << std::dec << idx << "/" << dis_out.getNumLines() << std::endl;
        ASSERT_EQ(exp_line, dis_line);
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
