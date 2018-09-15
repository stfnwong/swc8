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

SPECIAL_TOKENS:
    LD [I], V5
    LD F, VF
    LD B, V8
    LD VE, [I]

ARITHMETIC:     ; 8000 series instructions 
    LD VA, Vb
    OR V1, VF
    AND VC, VD
    XOR V3, V4
    ADD V7, V7
    SUB V3, V9
    SUBN V5, VA

RND_DRAW:
    RND VE, 0x56
    DRW VF, V4, 0x2

JMP_AND_CALL:
    CLS 
    RET
    SYS 0x040
    SYS 0xA1A
    CALL 0x443
