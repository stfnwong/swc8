/* TEST_CHIP8
 * Unit tests for Chip-8
 *
 * Stefan Wong 2018
 */

#include <vector>
#include <gtest/gtest.h>
#include "chip8.hpp"
// May need to assemble the program first, so bring in assembly tools 
#include "lexer.hpp"
#include "assembler.hpp"
#include "source.hpp"
#include "program.hpp"

class TestChip8 : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(TestChip8, test_init)
{
    Chip8 c8;
    std::vector<uint8_t> mem_dump = c8.dumpMem();
    ASSERT_EQ(0x12, mem_dump[0]);
    ASSERT_EQ(0x00, mem_dump[1]);
    for(unsigned int m = 2; m < mem_dump.size(); m++)
        ASSERT_EQ(0, mem_dump[m]);
}

TEST_F(TestChip8, test_instr_asm)
{
    int status;
    std::string prog_filename = "data/instr.asm";
    Chip8 c8;

    // Ensure the program is assembled, etc before execution 
    Lexer lexer;
    Assembler assembler;

    // Assemble the source into a program
    status = lexer.loadFile(prog_filename);
    ASSERT_EQ(0, status);
    std::cout << "\t Assembing file " << prog_filename << std::endl;
    lexer.lex();
    assembler.loadSource(lexer.getSourceInfo());
    assembler.assemble();

}

TEST_F(TestChip8, test_load_obj)
{
    int status;
    std::string asm_filename = "data/instr.asm";
    std::string obj_filename = "data/instr.obj";
    Lexer lexer;
    Assembler assembler;
    Program prog;
    Chip8 c8;

    std::cout << "Assembling file " << asm_filename << std::endl;
    lexer.loadFile(asm_filename);
    lexer.lex();
    assembler.loadSource(lexer.getSourceInfo());
    assembler.assemble();
    prog = assembler.getProgram();
    status = prog.writeObj(obj_filename);
    ASSERT_EQ(0, status);
    std::cout << "Wrote object output to " << obj_filename << std::endl;

    status = c8.loadMem(obj_filename, 0x200);
    ASSERT_EQ(0, status);

    std::vector<uint8_t> mem_contents = c8.dumpMem();
    //for(unsigned int idx = 0; idx < mem_contents.size(); ++idx)
    //    std::cout << std::hex << std::setw(2) << std::setfill('0') << mem_contents[idx] << " ";
    for(unsigned int idx = 0x200; idx < 0x230; ++idx)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << std::to_string(c8.readMem(idx)) << " ";
    std::cout << std::endl;

    // Check that memory contents do match at the program entry point
    std::cout << "Checking memory contents against program assembly..." << std::endl;
    Instr instr = prog.get(0);
    unsigned int start_adr = instr.adr;
    unsigned int mem_val; 
    for(unsigned int idx = start_adr; idx < prog.numInstr(); ++idx)
    {
        instr = prog.get(idx - start_adr);
        mem_val = c8.readMem(idx);
        ASSERT_EQ(instr.ins, mem_val);
    }
}

// Try to run the draw program (no display)
TEST_F(TestChip8, test_run_draw)
{
    int status;
    std::string asm_filename = "data/draw.asm";
    std::string obj_filename = "data/draw.obj";
    Lexer lexer;
    Assembler assembler;
    Program prog;
    Chip8 c8;

    std::cout << "Assembling file " << asm_filename << std::endl;
    lexer.loadFile(asm_filename);
    lexer.lex();
    assembler.loadSource(lexer.getSourceInfo());
    assembler.assemble();
    prog = assembler.getProgram();
    status = prog.writeObj(obj_filename);
    ASSERT_EQ(0, status);
    std::cout << "Wrote object output to " << obj_filename << std::endl;

    status = c8.loadMem(obj_filename, 0x200);
    ASSERT_EQ(0, status);

    // Now that the program binary is in memory, start 
    // invoking the cycle function and stepping through the 
    // instructions
    c8.setTrace(true);
    c8.cycle();         // jump to start address
    for(unsigned int i = 0; i < prog.numInstr(); ++i)
        c8.cycle();

    std::vector<C8Proc> state_trace = c8.getTrace();
    for(unsigned int i = 0; i < prog.numInstr(); ++i)
        std::cout << state_trace[i].toString() << std::endl;

}

// ===== INSTRUCTION UNIT TESTS ===== //
TEST_F(TestChip8, test_and_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD VF, 0x05
    // LD VA, 0x05
    // ADD VF, Va
    std::vector<uint8_t> test_data = {0x6F, 0x05, 0x6A, 0x05, 0x8F, 0xA2}; // ADD Vf, Va
    std::vector<C8Proc> expected_state;

    proc.pc = 0x200;
    proc.sp = 0;

    // Execute instruction
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < 2; ++i)
        c8.cycle();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
