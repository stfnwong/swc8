/* TEST_ASSEMBLER 
 * Tests for Chip8 psuedo-assembler 
 *
 * Stefan Wong 2018
 */

#include <vector>
#include <gtest/gtest.h>
#include "assembler.hpp"

class TestAssembler : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(TestAssembler, test_init)
{
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

