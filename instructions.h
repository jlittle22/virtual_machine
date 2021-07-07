/* Authors: Aryan Pandey (apande04) & John Little (jlittl04)
 * Data: November 2019
 * The Universal Machine
 * File: instructions.h - header file for the instructions package
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdlib.h>
#include <inttypes.h>
#include "bitpack.h"
#include "memory.h"

//Define the various widths and LSBs of the parts of an instruction
#define VAL_WIDTH 25
#define REG_WIDTH 3
#define OP_CODE_W 4
#define REGA_LSB 6
#define REGB_LSB 3
#define REGC_LSB 0
#define REGAprime_LSB 25
#define OP_CODE_LSB 28 
#define VAL_LSB 0

//Also important type definitions
typedef uint32_t umInstruction; 
typedef enum opCode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} opCode;
typedef uint8_t regID; 

/* extracting bits for opcodes, instructions, and registers */
umInstruction getNextInstruct(Segment program, uint32_t * prgmPtr); 
opCode getOpCode(umInstruction input); 
regID getRegA(umInstruction input); 
regID getRegB(umInstruction input); 
regID getRegC(umInstruction input); 
regID getRegAprime(umInstruction input); 
word getValue(umInstruction input); 
uint8_t checkAllow(word letter); 

/* executing operations */
void exConMove(word registers[], regID trgt, regID src, regID con); 
void exSegLoad(word registers[], regID trgt, regID seg, regID word, 
	           Seq_T memory); 
void exSegStore(word registers[], regID src, regID seg, regID wordC, 
	            Seq_T memory);
void exAddition(word registers[], regID trgt, regID ra, regID rb); 
void exMultiply(word registers[], regID trgt, regID ra, regID rb); 
void exDivide(word registers[], regID trgt, regID num, regID denom); 
void exBitNAND(word registers[], regID trgt, regID ra, regID rb); 
void exLoadVal(word registers[], regID trgt, umInstruction instruct); 
void halt(Seq_T memory, Seq_T segIDs, uint32_t * prgmPtr);
void mapSeg(word registers[], regID words, regID other, Seq_T segIDs,
            Seq_T memory); 
void unmapSeg(word registers[], regID c, Seq_T segIDs, Seq_T memory); 
void out(word registers[], regID output); 
void in(word registers[], regID input); 
void loadProgram(word registers[], regID b, regID c, Segment memory, 
	             uint32_t * prgmPtr); 

#endif
