; INSTR
; All instructions for Chip-8
; This program doesn't do anything. It just contains all instructions 
; in the instruction set at least once for testing the assembler and 
; disassembler
;
; Stefan Wong 2018

IMMEDIATE_SECTION:
    LD V1, 0x04
    LD VC, 0xAA

PARSE_VX_SECTION:
    SKP VA
    SKNP VB
