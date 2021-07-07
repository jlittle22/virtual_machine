/* Authors: John Little (jlittl04) & Aryan Pandey (apande04)
 * Date: November 2019
 * The Universal Machine
 * File: um.c - implementation for the UM
 */

#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdio.h>

#include "instructions.h"
#include "memory.h"
#include "bitpack.h"

/* Name: readFile
   Purpose: load the contents of a .um file into segment 0.
   Arguments: FILE pointer, length of file, segment 0
   Return: void 
*/
static inline void readFile(FILE * ptr, int length, Seq_T zero){
    assert(length % 4 == 0);
    assert(ptr != NULL);
    assert(zero != NULL);
    word * thing;
    uint32_t count = 0;
    /* extract and pack the bytes into instruction then put into segment 0*/
    while(count < (uint32_t)length){
        thing = malloc (sizeof(word));
        *thing = 0u;
        *thing |= getc(ptr) << 24;
        *thing |= (getc(ptr) << 16);
        *thing |= (getc(ptr) << 8);
        *thing |= (getc(ptr));
        Seq_addhi(zero, thing);
        count += 4;
    }
}

/* Name: orderOp
   Purpose: gets an instruction from the Segment 0, unpacks the instruction 
            to get the OP code and each register ID in use. Goes through
            a switch case to determine which operation needs to be called 
            and calls that operation. If operation is invalid, halts.
            Iterates program pointer.
   Arguments: memory sequence, segment IDs sequence, registers array, 
              program ptr.
   Return: void 
*/
static inline void orderOp(Seq_T memory, Seq_T segIDs, word registers[], 
                           uint32_t * prgmPtr){
    assert(memory != NULL);
    assert(segIDs != NULL);
    assert(registers != NULL);
    assert(prgmPtr != NULL);

    /* declare basic variables */
    regID regA = 0;
    regID regB = 0;
    regID regC = 0;
    opCode code = 0;
    int loop = 1;
    while(loop == 1){
        /* get next instruction and unpack opcode */
        umInstruction next = getNextInstruct(Seq_get(memory, 0), prgmPtr);
        code = getOpCode(next);
        /* switch case to execute whichever instruction is necessary */
        switch(code){
            case CMOV:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exConMove(registers, regA, regB, regC); 
                break;

            case SLOAD:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exSegLoad(registers, regA, regB, regC, memory); 
                break;

            case SSTORE:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exSegStore(registers, regC, regA, regB, memory);
                break;

            case ADD:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exAddition(registers, regA, regB, regC); 
                break;

            case MUL:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exMultiply(registers, regA, regB, regC);
                break;

            case DIV:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exDivide(registers, regA, regB, regC);
                break;

            case NAND:
                regA = getRegA(next);
                regB = getRegB(next);
                regC = getRegC(next);
                exBitNAND(registers, regA, regB, regC);
                break;

            case HALT:
                halt(memory, segIDs, prgmPtr);
                loop = 0;
                break;

            case MAP:
                regC = getRegC(next);
                regB = getRegB(next);
                mapSeg(registers, regC, regB, segIDs, memory);
                break;

            case UNMAP:
                regC = getRegC(next);
                unmapSeg(registers, regC, segIDs, memory); 
                break;

            case OUT:
                regC = getRegC(next);
                out(registers, regC);
                break;

            case IN:
                regC = getRegC(next);
                in(registers, regC); 
                break;

            case LOADP:
                regB = getRegB(next);
                regC = getRegC(next);
                loadProgram(registers, regB, regC, memory, prgmPtr);
                break;

            case LV:
                regA = getRegAprime(next);
                exLoadVal(registers, regA, next);
                break;

            default:
                /* if OP code is invalid, halt the machine */
                halt(memory, segIDs, prgmPtr);
        }
    }
}

int main(int argc, char* argv[]){
    /* Check command line usage */
    if (argc != 2){
        fprintf(stderr, "USAGE ERROR | Proper Usage:"); 
        fprintf(stderr, " ./um [UMBinaryFile].um\n");
        exit(EXIT_FAILURE);
    }

    /* declare and initialize an array of words to represent the registers */
    word registers[REG_COUNT];
    for (int i = 0; i < REG_COUNT; i++){
        registers[i] = 0;
    }

    /* declare and initialize our memory sequence and segment 0 
     * put segment 0 into memory 
     */                                 
    Seq_T memory = Seq_new(5);
    Seq_T zero = Seq_new(5);
    Seq_addlo(memory, zero);
    Seq_T segIDs = Seq_new(5);

    /* allocate space for program pointer */
    uint32_t * prgmPtr = malloc(sizeof(*prgmPtr));
    *prgmPtr = 0;

    /* read in the size of the UM binary file passed */
    struct stat st;
    stat(argv[1], &st);
    int length = st.st_size;

    /* open the UM binary file */
    FILE * binary = fopen(argv[1], "r");
    assert(binary != NULL);

    /* read the contents of the binary file in to segment 0 */
    readFile(binary, length, Seq_get(memory, 0));

    /* order operations for each instruction in segment 0 until all 
     * instructions have been executed properly
     */                     
    orderOp(memory, segIDs, registers, prgmPtr);

    /* close the binary file */
    fclose(binary);

    return 0;
}