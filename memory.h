/* Authors: Aryan Pandey (apande04) & John Little (jlittl04)
 * Data: November 2019
 * The Universal Machine
 * File: memory.h - header file for the memory package
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "uarray.h"
#include "seq.h"
#include <stdlib.h>
#include <inttypes.h>

//Define the expected num. of registers for our machine
#define REG_COUNT 8

//Important type definitions
typedef uint32_t word; 
typedef uint32_t umSegmentID;
typedef Seq_T Segment;


//function contracts for memory management
void editWord(Seq_T memory, umSegmentID id, uint32_t offset, word insert);
word getWordat(umSegmentID seg, uint32_t offset, Seq_T memory); 
void allocate(uint32_t wordCount, umSegmentID id, Seq_T memory); 
void deallocate(Segment sgmnt); 
void completeFree(Seq_T memory, Seq_T segIDs); 
void pushSegID(Seq_T trgt, umSegmentID oldID); 
umSegmentID chooseID(Seq_T segIDs, Seq_T memory); 




#endif