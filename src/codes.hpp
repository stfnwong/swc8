/*
 * CODES
 * Lexer codes, instruction codes, etc
 *
 * Stefan Wong 2019
 */

#ifndef __CODES_HPP
#define __CODES_HPP

#include "opcode.hpp"

// Special register flags 
#define LEX_IREG  0x01
#define LEX_BREG  0x02
#define LEX_FREG  0x04
#define LEX_KREG  0x08
#define LEX_DTREG 0x10
#define LEX_STREG 0x20
#define LEX_IST   0x40  // for LD [I] Vx
#define LEX_ILD   0x80  // for LD Vx [I]

// Opcode flags 
#define LEX_OP_IMM   0x01
#define LEX_OP_LABEL 0x02
#define LEX_OP_DIR   0x04

// Instruction codes - not quite opcodes
typedef enum instr_code{ 
    LEX_SYS, LEX_CLS, LEX_RET, LEX_JP, LEX_CALL, LEX_SE, LEX_SNE, 
    LEX_LD, LEX_ADD, LEX_OR, LEX_AND, LEX_XOR, LEX_SUB, LEX_SHR, 
    LEX_SUBN, LEX_SHL, LEX_RND, LEX_DRW, LEX_SKP, LEX_SKNP,
    LEX_DW, LEX_DB
} instr_code;


// Instruction codes for Lexer 
const Opcode lex_instr_codes[] = {
    {LEX_SYS,  "SYS"},
    {LEX_CLS,  "CLS"},
    {LEX_RET,  "RET"},
    {LEX_JP,   "JP"},
    {LEX_CALL, "CALL"},
    {LEX_SE,   "SE"},
    {LEX_SNE,  "SNE"},
    {LEX_LD,   "LD"},
    {LEX_ADD,  "ADD"},
    {LEX_OR,   "OR"},
    {LEX_AND,  "AND"},
    {LEX_XOR,  "XOR"},
    {LEX_SUB,  "SUB"},
    {LEX_SHR,  "SHR"},
    {LEX_SUBN, "SUBN"},
    {LEX_SHL,  "SHL"},
    {LEX_RND,  "RND"},
    {LEX_DRW,  "DRW"},
    {LEX_SKP,  "SKP"},
    {LEX_SKNP, "SKNP"},
    {LEX_DW,   "DW"},
    {LEX_DB,   "DB"}
};


#endif /*__CODES_HPP*/
