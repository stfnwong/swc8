/*
 * MEMORY
 * Stuff for working with memory 
 *
 * Stefan Wong 2019
 */

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "memory.hpp"

/*
 * mem_line_to_string()
 */
std::string mem_line_to_string(int addr, uint8_t byte1, uint8_t byte2)
{
    std::ostringstream oss;

    // address 
    oss << "  0x" << std::hex << std::setw(4) << std::setfill('0')
        << std::uppercase << unsigned(addr);
    // spacer 
    oss << " |  ";
    // data
    oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << unsigned(byte1);
    oss << " ";
    oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << unsigned(byte2);

    return oss.str();
}


/*
 * mem_to_string()
 */
std::string mem_to_string(const uint8_t* data, int length, int start_offset, bool skip_zeros)
{
    std::ostringstream oss;

    // Print some preamble 
    oss << "Printing " << unsigned(length) << " bytes starting at offset [0x" 
        << std::hex << std::setw(4) << std::setfill('0') << std::uppercase
        << unsigned(start_offset) << "]" << std::endl;

    oss << "---------------------" << std::endl;
    oss << "  ADDR      DATA  " << std::endl;
    oss << "---------------------" << std::endl;

    int cur_addr = start_offset;
    int zeros_skipped = 0;
    // each 'line' in the output is two bytes long
    for(int b = 0; b < length; ++b)
    {
        if((b > 0) && (b % 2 == 0))
        {
            if(skip_zeros)
            {
                if((data[b] == 0) && (data[b+1] == 0))
                {
                    zeros_skipped++;
                    goto ADVANCE_ADDR;
                }
            }
            oss << mem_line_to_string(cur_addr, data[b], data[b+1]);
            oss << std::endl;
        }
ADVANCE_ADDR:
        cur_addr++;
    }
    oss << std::endl;
    if(skip_zeros)
        oss << "Skipped " << std::dec << unsigned(zeros_skipped) << " zeros in memory" << std::endl;

    return oss.str();
}


void print_mem_dump(const uint8_t* data, int length, int start_offset, bool skip_zeros)
{
    std::string mem_string = mem_to_string(data, length, start_offset, skip_zeros);
    std::cout << mem_string << std::endl;
}
