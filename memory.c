/* Authors: Aryan Pandey (apande04) & John Little (jlittl04)
 * Data: November 2019
 * The Universal Machine
 * File: memory.c - implementation for the memory package 
 */

#include "memory.h"
#include <assert.h>
#include <stdio.h>

/* Name: getWord 
   Purpose: accesses a given segment and word offset. Returns that word 
            retrieved from memory.
   Arguments: segment id, offset, memory sequence
   Return: word
*/
word getWordat(umSegmentID seg, uint32_t offset, Seq_T memory){
    assert(memory != NULL);
    /* access specified segment then the word withn that segment */
    Segment sgmnt = (Segment) Seq_get(memory, (int)seg);
    word * value = (word *) Seq_get(sgmnt, (int)offset);

    return *value;
}

/* Name: deallocate
   Purpose: deallocates each word in a given segment and the segment itself
   Arguments: a Segment 
   Return: void 
*/
void deallocate(Segment sgmnt){
    assert(sgmnt != NULL);
    int len = Seq_length(sgmnt);
    /* free each word in segment */
    for (int i = 0; i < len; i++){
        free((word *)Seq_get(sgmnt, i));
    }
    Seq_free(&sgmnt);
}

/* Name: completeFree
   Purpose: completely frees all memory associated with the 
            machine (except the FILE * in main). This includes the sequence
            of available segment IDs and the main memory block itself. 
   Arguments: memory sequence, segIDs sequence
   Return: void
*/
void completeFree(Seq_T memory, Seq_T segIDs){
    assert(memory != NULL);
    assert(segIDs != NULL);

    /* free each segment in memory */
    int len = Seq_length(memory);
    for (int i = 0; i < len; i++){
        Seq_T thing = Seq_get(memory, i);
        if (thing != NULL){
            deallocate(thing);
        }
    }
    Seq_free(&memory);

    /* free each unused segment ID */
    int length = Seq_length(segIDs);
    for (int i = 0; i < length; i++) {
        free((umSegmentID *) Seq_get(segIDs, i));
    }
    Seq_free(&segIDs);
}

/* Name: allocate
   Purpose: allocate a block of memory with enough word spaces and a given 
            ID. 
   Arguments: word count, a segment ID, and memory sequence
   Return: void
*/
void allocate(uint32_t wordCount, umSegmentID id, Seq_T memory){
    assert(memory != NULL);

    /* allocated a new block of size word for each word we want */
    Segment seg = Seq_new(wordCount);
    for (uint32_t i = 0; i < wordCount; i++){
        word * thing = malloc(sizeof(word));
        assert(thing != NULL);
        Seq_addhi(seg, thing);
        *thing = 0;
    }

    /* add new segment to memory -- avoid triggering Hanson assert */
    if(id == (uint32_t)Seq_length(memory)){
        Seq_addhi(memory, seg);
    }
    else{
        Seq_put(memory, id, seg);
    }
}

/* Name: chooseID
   Purpose: generates an ID for a new segment 
   Arguments: sequence of available IDs, memory sequence
   Return: umSegmentID
*/
umSegmentID chooseID(Seq_T segIDs, Seq_T memory){
    assert(memory != NULL);

    /* select an ID for a new segment */
    umSegmentID id = 0;
    if (Seq_length(segIDs) != 0){
        umSegmentID * thing = (umSegmentID *)Seq_remlo(segIDs);
        id = *thing;
        free(thing);
    }
    else {
        id = Seq_length(memory);
    }

    return id;
}

/* Name: pushSegID 
   Purpose: add a segment ID into the availabe IDs sequence
   Arguments: seguence of available IDs, an old segment ID
   Return: void
*/
void pushSegID(Seq_T trgt, umSegmentID oldID){
    assert(trgt != NULL);

    /* allocate space for new id and add */
    uint32_t * new = malloc(sizeof(*new));
    assert(new != NULL);
    *new = oldID;
    Seq_addhi(trgt, new);
}

/* Name: editWord
   Purpose: edits a specific word inside of a memory segment
   Arguments: memory sequence, segment ID, offset of words, new word to add
   Return: void 
*/
void editWord(Seq_T memory, umSegmentID id, uint32_t offset, word insert){
    assert(memory != NULL);
    word * thing = (word *)Seq_get(Seq_get(memory, id), (int)offset);
    *thing = insert;
}