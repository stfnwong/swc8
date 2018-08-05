/* TEST_CHIP8
 * Unit tests for Chip-8
 *
 * Stefan Wong 2018
 */

#include <vector>
#include <gtest/gtest.h>
#include "chip8.hpp"

class TestChip8 : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(TestChip8, test_init)
{
    Chip8 c8;
    std::vector<uint8_t> mem_dump = c8.dumpMem();
    for(unsigned int m = 0; m < mem_dump.size(); m++)
        ASSERT_EQ(0, mem_dump[m]);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
