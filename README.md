# C8
## A Chip-8 emulator

Everyone has tried to write one of these right? So why not me? WELL HERE IT IS.

This is a Chip-8 emulator written in C++. Includes some tools (eg: assembler, disassembler)

# Using it
Its not really complete, so it can't be *used* properly as of this writing, but a `make all` is supposed to
- Build the assembler
- Build the disassembler
- If I get around to making a special debugger, then build that
- Build a command line program to actually run the emulation

At which point you would do something like  `./c8 input.asm <options>` to run it.

# Requirements
- Uses gtest for unit tests
- SDL for drawing (not in master branch yet though)
- I used GCC 9.1.1, but anything with C++14 support should work.
- Only requires C++14 because I put it in the Makefile, really its not even a fancy enough implementation to *need* anything in C++14 (in fact, it may as well be written in C).

# TODOs
- Finish the rendering part
- Test more asms
- Maybe even write some kind of n-fangled live memory trace/breakpoint thing? Who knows
- *More to come*
