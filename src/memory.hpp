/*
 * MEMORY
 * Stuff for working with memory 
 *
 * Stefan Wong 2019
 */

#ifndef __MEMORY_HPP
#define __MEMORY_HPP


// Something to nicely format a memory dump

std::string mem_to_string(const uint8_t* data, int length, int start_offset, bool skip_zeros);
void print_mem_dump(const uint8_t* data, int length, int start_offset, bool skip_zeros);


#endif /*__MEMORY_HPP*/
