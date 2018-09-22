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

    // State vector compare 
    public:
        void compare_state_vector(
                const std::vector<C8Proc>& exp_vector,
                const std::vector<C8Proc>& out_vector);
};

void TestChip8::compare_state_vector(
        const std::vector<C8Proc>& exp_vector,
        const std::vector<C8Proc>& out_vector)
{
    //ASSERT_EQ(exp_vector.size(), out_vector.size());
    bool eq;
    for(unsigned int i = 0; i < exp_vector.size(); ++i)
    {
        C8Proc exp_proc = exp_vector[i];
        C8Proc out_proc = out_vector[i];
        eq = (exp_proc == out_proc);
        if(!eq)
        {
            std::cout << "Error in State " << i << ":" << std::endl;
            std::cout << out_proc.toString();
            std::cout << "State " << i << " diff:" << std::endl;
            std::cout << out_proc.diffStr(exp_proc) << std::endl;
            //std::cout << out_vector[i].diffStr(exp_proc[i]) << std::endl;
        }
        ASSERT_EQ(true, eq);
    }
}

// Test initialization
TEST_F(TestChip8, test_init)
{
    Chip8 c8;
    std::vector<uint8_t> mem_dump = c8.dumpMem();
    ASSERT_EQ(0x12, mem_dump[0]);
    ASSERT_EQ(0x00, mem_dump[1]);
    for(unsigned int m = 2; m < mem_dump.size(); m++)
        ASSERT_EQ(0, mem_dump[m]);

    // Check that the state is initialized correctly 
    C8Proc state = c8.getState(); 

    // registers, stack
    for(int r = 0; r < 16; ++r)
        ASSERT_EQ(0, state.V[r]);

    for(int s = 0; s < 12; ++s)
        ASSERT_EQ(0, state.stack[s]);
    // rest of state 
}

//TEST_F(TestChip8, test_instr_asm)
//{
//    int status;
//    std::string prog_filename = "data/instr.asm";
//    Chip8 c8;
//
//    // Ensure the program is assembled, etc before execution 
//    Lexer lexer;
//    Assembler assembler;
//
//    // Assemble the source into a program
//    status = lexer.loadFile(prog_filename);
//    ASSERT_EQ(0, status);
//    std::cout << "\t Assembing file " << prog_filename << std::endl;
//    lexer.lex();
//    assembler.loadSource(lexer.getSourceInfo());
//    assembler.assemble();
//
//}

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
// Arithmetic instructions 

// ADD 
TEST_F(TestChip8, test_add_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD V3, 0x05
    // LD VA, 0x05
    // ADD V3, Va
    std::vector<uint8_t> test_data = {0x63, 0x05, 0x6A, 0x05, 0x83, 0xA4}; 
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x3] = 0x05;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x3] = 0x05;
    proc.V[0xA] = 0x05;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x3] = 0x05 + 0x5;
    proc.V[0xA] = 0x05;
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    bool eq;
    for(unsigned int i = 0; i < expected_state.size(); ++i)
    {
        C8Proc exp_proc = expected_state[i];
        C8Proc out_proc = trace_out[i];
        eq = exp_proc == out_proc;
        ASSERT_EQ(true, eq);
    }
}

// ADDI
TEST_F(TestChip8, test_add_vxkk)
{
    Chip8 c8;
    C8Proc proc;

    // LD V3, 0xFF
    // ADD V3, 0xFF
    std::vector<uint8_t> test_data = {0x63, 0xFF, 0x73, 0xFF};
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x3] = 0xFF;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x3] = 0xFE;     // no overflow flag for ADDI
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    this->compare_state_vector(expected_state, trace_out);

    //bool eq;
    //for(unsigned int i = 0; i < expected_state.size(); ++i)
    //{
    //    C8Proc exp_proc = expected_state[i];
    //    C8Proc out_proc = trace_out[i];
    //    eq = exp_proc == out_proc;
    //    if(!eq)
    //        std::cout << trace_out[i].toString();
    //    ASSERT_EQ(true, eq);
    //}
}

// AND 
TEST_F(TestChip8, test_and_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD V2, 0x0A
    // LD V3, 0xE4
    // AND V3, V2
    std::vector<uint8_t> test_data = {0x62, 0x0A, 0x63, 0xE4, 0x83, 0x22}; // AND V3, V2
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x2] = 0x0A;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x2] = 0x0A;
    proc.V[0x3] = 0xE4;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x2] = 0x0A;
    proc.V[0x3] = 0x0A & 0xE4;
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    bool eq;
    for(unsigned int i = 0; i < expected_state.size(); ++i)
    {
        C8Proc exp_proc = expected_state[i];
        C8Proc out_proc = trace_out[i];
        eq = exp_proc == out_proc;
        ASSERT_EQ(true, eq);
    }
}

// OR 
TEST_F(TestChip8, test_or_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD V3, 0xBA
    // LD V4, 0xEF
    // OR V3, V4
    std::vector<uint8_t> test_data = {0x63, 0xBA, 0x64, 0xEF, 0x83, 0x41};
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x3] = 0xBA;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x3] = 0xBA;
    proc.V[0x4] = 0xEF;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x3] = 0xBA | 0xEF;
    proc.V[0x4] = 0xEF;
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    bool eq;
    for(unsigned int i = 0; i < expected_state.size(); ++i)
    {
        C8Proc exp_proc = expected_state[i];
        C8Proc out_proc = trace_out[i];
        eq = exp_proc == out_proc;
        if(!eq)
        {
            std::cout << "State " << i << ":" << std::endl << trace_out[i].toString();
            std::cout << trace_out[i].diffStr(expected_state[i]);
        }
        ASSERT_EQ(true, eq);
    }
}

// XOR
TEST_F(TestChip8, test_xor_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD V1, 0xAA
    // LD V6, 0xBB
    // XOR V1, V6
    std::vector<uint8_t> test_data = {0x61, 0xAA, 0x66, 0xBB, 0x81, 0x63}; 
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x1] = 0xAA;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x1] = 0xAA;
    proc.V[0x6] = 0xBB;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x1] = 0xAA ^ 0xBB;
    proc.V[0x6] = 0xBB;
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    bool eq;
    for(unsigned int i = 0; i < expected_state.size(); ++i)
    {
        C8Proc exp_proc = expected_state[i];
        C8Proc out_proc = trace_out[i];
        eq = exp_proc == out_proc;
        if(!eq)
        {
            std::cout << "State " << i << ":" << std::endl << trace_out[i].toString();
            std::cout << trace_out[i].diffStr(expected_state[i]);
        }
        ASSERT_EQ(true, eq);
    }
}

// SUB
TEST_F(TestChip8, test_sub_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD V3, 0xFF
    // LD VE, 0xEF
    // SUB V3, VE
    std::vector<uint8_t> test_data = {0x63, 0xFF, 0x6E, 0xEF, 0x83, 0xE5};
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x3] = 0xFF;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x3] = 0xFF;     
    proc.V[0xE] = 0xEF;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x3] = 0xFF - 0xEF;
    proc.V[0xE] = 0xEF;
    proc.V[0xF] = 0x1;
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    bool eq;
    for(unsigned int i = 0; i < expected_state.size(); ++i)
    {
        C8Proc exp_proc = expected_state[i];
        C8Proc out_proc = trace_out[i];
        eq = exp_proc == out_proc;
        if(!eq)
        {
            std::cout << "State " << i << ":" << std::endl << trace_out[i].toString();
            std::cout << trace_out[i].diffStr(expected_state[i]);
        }
        ASSERT_EQ(true, eq);
    }
}

// SUBN
TEST_F(TestChip8, test_subn_vxvy)
{
    Chip8 c8;
    C8Proc proc;

    // LD V3, 0x08
    // LD VE, 0x04
    // SUBN V3, VE
    std::vector<uint8_t> test_data = {0x63, 0x08, 0x6E, 0x04, 0x83, 0xE7};
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x3] = 0x08;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x3] = 0x08;
    proc.V[0xE] = 0x04;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x3] = 0x4 - 0x8;
    proc.V[0xE] = 0x04;
    proc.V[0xF] = 0x1;      // not sure about this carry flag....
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    std::vector<C8Proc> trace_out = c8.getTrace();

    bool eq;
    for(unsigned int i = 0; i < expected_state.size(); ++i)
    {
        C8Proc exp_proc = expected_state[i];
        C8Proc out_proc = trace_out[i];
        eq = exp_proc == out_proc;
        if(!eq)
        {
            std::cout << "State " << i << ":" << std::endl << trace_out[i].toString();
            std::cout << trace_out[i].diffStr(expected_state[i]);
        }
        ASSERT_EQ(true, eq);
    }
}


// PROGRAM CONTROL
//TEST_F(TestChip8, test_jp)
//{
//    Chip8 c8;
//    C8Proc proc;
//
//    // LD VE, 0x1
//    // LD V2, VE
//    // ADD VE, 0x1
//    // LD V3, VE
//    // ADD VE, 0x1
//    // ... and so on through 
//    // SUBN V3, VE
//    std::vector<uint8_t> test_data = {0x63, 0x08, 0x6E, 0x04, 0x83, 0xE7};
//    std::vector<C8Proc> expected_state;
//
//    // Create the expected state sequence
//    proc.init();
//    proc.pc     = 0x200;
//    expected_state.push_back(proc);
//    proc.init();
//    proc.pc     = 0x202;
//    proc.V[0x3] = 0x08;
//    expected_state.push_back(proc);
//    proc.init();
//    proc.pc     = 0x204;
//    proc.V[0x3] = 0x08;
//    proc.V[0xE] = 0x04;
//    expected_state.push_back(proc);
//    proc.init();
//    proc.pc     = 0x206;
//    proc.V[0x3] = 0x4 - 0x8;
//    proc.V[0xE] = 0x04;
//    proc.V[0xF] = 0x1;      // not sure about this carry flag....
//    expected_state.push_back(proc);
//
//    // Execute instruction
//    c8.setTrace(true);
//    c8.loadMem(test_data, 0x200);
//    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
//        c8.cycle();
//    std::vector<C8Proc> trace_out = c8.getTrace();
//
//    bool eq;
//    for(unsigned int i = 0; i < expected_state.size(); ++i)
//    {
//        C8Proc exp_proc = expected_state[i];
//        C8Proc out_proc = trace_out[i];
//        eq = exp_proc == out_proc;
//        if(!eq)
//        {
//            std::cout << "State " << i << ":" << std::endl << trace_out[i].toString();
//            std::cout << trace_out[i].diffStr(expected_state[i]);
//        }
//        ASSERT_EQ(true, eq);
//    }
//}

TEST_F(TestChip8, test_call_ret)
{
    Chip8 c8;
    C8Proc proc;
    std::vector<C8Proc> trace_out;

    // LD V3, 0x08
    // LD V4, 0x04
    // ADD V3, V4
    // CALL 0x20A
    // LD V5 0x01
    // ADD V3, V5
    // RET
    // LD V6, 0xFF
    std::vector<uint8_t> test_data = {
        0x63, 0x08, 0x64, 0x04, 0x83, 0x44, 0x22, 0x0A,
        0x65, 0x01, 0x83, 0x54, 0x00, 0xEE, 0x66, 0xFF};
    std::vector<C8Proc> expected_state;

    // Create the expected state sequence
    proc.init();
    proc.pc     = 0x200;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x202;
    proc.V[0x3] = 0x08;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x204;
    proc.V[0x3] = 0x08;
    proc.V[0x4] = 0x04;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x206;
    proc.V[0x3] = 0x8 + 0x4;
    proc.V[0x4] = 0x04;
    expected_state.push_back(proc);
    // Call 
    proc.init();
    proc.pc     = 0x208;
    proc.V[0x3] = 0xB;
    proc.V[0x4] = 0x04;
    proc.stack[0] = 0x208;
    proc.sp = 1;
    expected_state.push_back(proc);
    // "subroutine"
    proc.init();
    proc.pc     = 0x20A;
    proc.V[0x3] = 0xB;
    proc.V[0x4] = 0x04;
    proc.V[0x5] = 0x01;
    proc.stack[0] = 0x208;
    proc.sp = 1;
    expected_state.push_back(proc);
    proc.init();
    proc.pc     = 0x20C;
    proc.V[0x3] = 0xC;
    proc.V[0x4] = 0x04;
    proc.V[0x5] = 0x01;
    proc.stack[0] = 0x208;
    proc.sp = 1;
    expected_state.push_back(proc);
    // RET
    proc.init();
    proc.pc     = 0x20E;
    proc.V[0x3] = 0xC;
    proc.V[0x4] = 0x04;
    proc.V[0x5] = 0x01;
    proc.sp = 0;
    expected_state.push_back(proc);
    // Move to previous stack pointer 
    // positon
    proc.init();
    proc.pc     = 0x208;
    proc.V[0x3] = 0xC;
    proc.V[0x4] = 0x04;
    proc.V[0x5] = 0x01;
    expected_state.push_back(proc);

    // Execute instruction
    c8.setTrace(true);
    c8.loadMem(test_data, 0x200);
    for(unsigned int i = 0; i < expected_state.size()+1; ++i)
        c8.cycle();
    trace_out = c8.getTrace();

    // TODO : (debug) dump the whple trace here 
    for(unsigned int t = 0; t < 8; ++t)
        std::cout << trace_out[t].toString();

    this->compare_state_vector(expected_state, trace_out);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
