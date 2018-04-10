/*! \file
 *
 * This file contains definitions for an Arithmetic/Logic Unit of an
 * emulated processor.
 */


#include <stdio.h>
#include <stdlib.h>   /* malloc(), free() */
#include <string.h>   /* memset() */

#include "alu.h"
#include "instruction.h"


/*!
 * This function dynamically allocates and initializes the state for a new ALU
 * instance.  If allocation fails, the program is terminated.
 */
ALU * build_alu() {
    /* Try to allocate the ALU struct.  If this fails, report error then exit. */
    ALU *alu = malloc(sizeof(ALU));
    if (!alu) {
        fprintf(stderr, "Out of memory building an ALU!\n");
        exit(11);
    }

    /* Initialize all values in the ALU struct to 0. */
    memset(alu, 0, sizeof(ALU));
    return alu;
}


/*! This function frees the dynamically allocated ALU instance. */
void free_alu(ALU *alu) {
    free(alu);
}


/*!
 * This function implements the logic of the ALU.  It reads the inputs and
 * opcode, then sets the output accordingly.  Note that if the ALU does not
 * recognize the opcode, it should simply produce a zero result.
 */
void alu_eval(ALU *alu) {
    uint32_t A, B, aluop, temp;
    uint32_t result;

    A = pin_read(alu->in1);
    B = pin_read(alu->in2);
    aluop = pin_read(alu->op);

    result = 0;

    switch(aluop){
        case ALUOP_POS:
        break;

        case ALUOP_MASK:
        break;

        case ALUOP_ADD :
        result = A + B;
        break;
           /*!< The opcode for simple addition. */
        case ALUOP_INV :
        result = ~A;
        break;
           /*!< The opcode for bitwise invert. */
        case ALUOP_SUB :
        result = A - B;
        break;
           /*!< The opcode for simple subtraction. */
        case ALUOP_XOR :
        result = A ^ B;
        break;
           /*!< The opcode for bitwise exclusive-or. */
        case ALUOP_OR:
        result = A | B;
        break;
           /*!< The opcode for bitwise or. */
        case ALUOP_INCR:
        result = A + 1;
        break;
           /*!< The opcode for incrementing a register. */
        case ALUOP_AND :
        result = A & B;
        break;
           /*!< The opcode for bitwise and. */
        case ALUOP_SRA :
        result = A >> 1;
        temp = A & (0x80000000);
        if(temp > 0){
            result = result | 0x80000000;
        }
        break;
           /*!< The opcode for arithmetic shift-right. */
        case ALUOP_SRL :
        result = A >> 1;
        break;
           /*!< The opcode for logical shift-right. */
        case ALUOP_SLA :
        result = A << 1;
        break;

        case ALUOP_SLL :
        result = A << 1;
        break;

        default:
        result = 0;
        break;
    }

    pin_set(alu->out, result);
}