/*
 Authors: Aryan Pandey (apande04) & John Little (jlittl04)
 * Data: November 2019
 * The Universal Machine
 * File: instructions.c - implementation for the instructions package
 */

#include "instructions.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Name: checkAllow
   Purpose: check if a word value is acceptably read as a char
   Arguments: word 
   Return: uint8_t -- returns 0 if false, 1 if true
*/
uint8_t checkAllow(word letter){
	if (letter > 255){
		return 0;
	}
	else {
		return 1;
	}
}

/* Name: exConMove
   Purpose: executes the conditional move operation
   Arguments: array of words rep. registers and 3 register IDs
   Return: Void
*/
void exConMove(word registers[], regID trgt, regID src, regID con){
	assert(registers != NULL);
	if (registers[con] != 0){
		registers[trgt] = registers[src];
	}
}

/* Name: exSegLoad
   Purpose: executes the segment load operation
   Arguments: array for registers, 3 register IDs, memory Sequence
   Return: void
*/
void exSegLoad(word registers[], regID trgt, regID seg, regID word, 
			   Seq_T memory){
	assert(registers != NULL);
	assert(memory != NULL);

	uint32_t x = registers[seg]; 
	uint32_t offset = registers[word]; 
	registers[trgt] = getWordat(x, offset, memory);
}

/* Name: exAddition
   Purpose: execute addition operation
   Arguments: array for registers, 3 register IDs
   Return: void
*/
void exAddition(word registers[], regID trgt, regID ra, regID rb){ 
	assert(registers != NULL);
	registers[trgt] = ((registers[ra] + registers[rb]) % 
					   ((uint64_t)UINT_MAX + 1));
}

/* Name: exMultiply 
   Purpose: execute multiply operation
   Arguments: registers array, 3 register IDs
   Return: void
*/
void exMultiply(word registers[], regID trgt, regID ra, regID rb){
	assert(registers != NULL);
	registers[trgt] = ((registers[ra] * registers[rb]) % 
		               ((uint64_t)UINT_MAX + 1));
}

/* Name: exDivide
   Purpose: execute divide operation
   Arguments: registers array, 3 register IDs
   Return: void
*/
void exDivide(word registers[], regID trgt, regID num, regID denom){
	assert(registers != NULL);
	registers[trgt] = ((registers[num] / registers[denom]));	
}

/* Name: exBitNAND
   Purpose: execute bitwise NAND operation
   Arguments: registers array, 3 register IDs
   Return: void
*/
void exBitNAND(word registers[], regID trgt, regID ra, regID rb){
	assert(registers != NULL);
	registers[trgt] = ~(registers[ra] & registers[rb]);
}

/* Name: out
   Purpose: check if contents of a register are interpretable as a char and 
   			print a char to standard out 
   Arguments: registers array, 1 register ID
   Return: void
*/
void out(word registers[], regID output){
	assert(registers != NULL);
	if (!checkAllow(registers[output])){
		return;
	}
	printf("%c", registers[output]);
}

/* Name: in
   Purpose: wait for input from stdin... once arrived set register 
            equal to input
   Arguments: registers array, 1 register ID
   Return: void
*/
void in(word registers[], regID input){
	assert(registers != NULL);
	word in = 0;
	in = getchar();
	if (checkAllow(in)){
		registers[input] = in;
	}
	else {
		registers[input] = 0xFFFFFFFF;
	}
}

/*-------------------------------------------------*/
/* SEE memory.c FOR MORE INFO ON MEMORY MANAGEMENT */
/*-------------------------------------------------*/

/* Name: mapSeg
   Purpose: executed the map segment operation 
   Arguments: registers array, 2 register IDs, segment IDs sequence, memory
   Return: void
*/
void mapSeg(word registers[], regID words, regID other, Seq_T segIDs, 
	        Seq_T memory){
	assert(registers != NULL);
	assert(segIDs != NULL);
	assert(memory != NULL);

	/* generate new id for segment, then allocate a new block of memory */
	uint32_t count = registers[words];
	umSegmentID id = chooseID(segIDs, memory);
	allocate(count, id, memory);
	registers[other] = id;
}

/* Name: unmapSeg 
   Purpose: execute the unmap segment operation
   Arguments: registers array, 2 register IDs, segmnet IDs sequence, memory
   Return: void
*/
void unmapSeg(word registers[], regID c, Seq_T segIDs, Seq_T memory){
	assert(registers != NULL);
	assert(segIDs != NULL);
	assert(memory != NULL);

	/* put segment ID into availabe sequence and deallocate segment */
	pushSegID(segIDs, registers[c]);
	deallocate((Segment)Seq_get(memory, registers[c]));
	Seq_put(memory, registers[c], NULL);
}

/* Name: exLoadVal
   Purpose: execture load value operation
   Arguments: registers array, 1 register ID, a UM instruction
   Return: void
*/
void exLoadVal(word registers[], regID trgt, umInstruction instruct){
	assert(registers != NULL);
	registers[trgt] = getValue(instruct);
}

/* Name: getOpCode
   Purpose: unpacks the OP code bit from an instruction
   Arguments: a UM instruction
   Return: opCode
*/
opCode getOpCode(umInstruction input){
	return (opCode)Bitpack_getu(input, OP_CODE_W, OP_CODE_LSB);
}

/* Name: getRegA
   Purpose: unpacks the register A bits from an instruction
   Arguments: a UM instruction
   Return: regID
*/
regID getRegA(umInstruction input){
	return (regID)Bitpack_getu(input, REG_WIDTH, REGA_LSB);
}

/* Name: getRegB
   Purpose: unpack the register B bits from an instruction
   Arguments: a UM instruction
   Return: regID
*/
regID getRegB(umInstruction input){
	return (regID)Bitpack_getu(input, REG_WIDTH, REGB_LSB);
}

/* Name: getRegC
   Purpose: unpack the register C bits from an instruction
   Arguments: a UM instruction
   Return: regID
*/
regID getRegC(umInstruction input){
	return (regID)Bitpack_getu(input, REG_WIDTH, REGC_LSB);
}

/* Name: getRegAprime
   Purpose: unpack the register A bits from LOAD VALUE instruction
   Arguments: a UM instruction
   Return: regID
*/
regID getRegAprime(umInstruction input){
	return (regID)Bitpack_getu(input, REG_WIDTH, REGAprime_LSB);
}

/* Name: getValue
   Purpose: unpacks the value from a LOAD VALUE instruction
   Arguments: a UM instruction
   Return: word
*/
word getValue(umInstruction input){
	return (word)Bitpack_getu(input, VAL_WIDTH, VAL_LSB);
}

/* Name: getNextInstruct
   Purpose: returns the next instruction for machine to execute from segment 0.
   Arguments: a segment, the program pointer
   Return: umInstruction
*/
umInstruction getNextInstruct(Segment program, uint32_t * prgmPtr){
	assert(program != NULL);
	assert(prgmPtr != NULL);

	umInstruction thing; 
	thing = *(umInstruction *)Seq_get(program, *prgmPtr);
	(*prgmPtr)++;
	return thing;
}

/* Name: loadProgram
   Purpose: load a given segment into segment 0 and reset the program 
   			pointer to a given value.
   Arguments: registers array, 2 register IDs, memory segment, program pointer
   Return: void 
*/
void loadProgram(word registers[], regID b, regID c, Segment memory, 
				 uint32_t * prgmPtr){
	assert(registers != NULL);
	assert(prgmPtr != NULL);
	assert(memory != NULL);

	/* check if the segment being loaded is 0 */
	if (registers[b] == 0){
		*prgmPtr = registers[c];
		return;
	}

	/* make a deep copy of the segment */
	umSegmentID i = registers[b];
	Segment src = Seq_get(memory, i);
	int len = Seq_length(Seq_get(memory, i));
	Segment copy = Seq_new(len);
	for (int j = 0; j < len; j++){
		word * thing = malloc(sizeof(*thing));
		*thing = *(word *)Seq_get(src, j);
		Seq_addhi(copy, thing);
	}

	/* put the deep copy into the segment 0 slot */
	Segment old = Seq_get(memory, 0);
	deallocate(old);
	Seq_put(memory, 0, copy);

	*prgmPtr = registers[c];
}

/* Name: halt
   Purpose: stop the machine and clean up memory
   Arguments: memory sequence, segIDs sequence, and program pointer
   Return: void 
*/
void halt(Seq_T memory, Seq_T segIDs, uint32_t * prgmPtr){
	assert(memory != NULL);
	assert(segIDs != NULL);
	assert(prgmPtr != NULL);

	completeFree(memory, segIDs);
	free(prgmPtr);
}

/* Name: exSegStore
   Purpose: execute segment store operation
   Arguments: registers array, 3 register IDs, memory sequence
   Return: void
*/
void exSegStore(word registers[], regID src, regID seg, regID wordC, 
	            Seq_T memory){
	assert(registers != NULL);
	assert(memory != NULL);

	umSegmentID id = registers[seg];
	word offset = registers[wordC];
	word addie = registers[src];
	editWord(memory, id, offset, addie);

}
