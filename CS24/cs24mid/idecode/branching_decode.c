/*! \file
 *
 * This file contains the definitions for the instruction decoder for the
 * branching processor.  The instruction decoder extracts bits from the current
 * instruction and turns them into separate fields that go to various functional
 * units in the processor.
 */

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "branching_decode.h"
#include "register_file.h"
#include "instruction.h"


/*
 * Branching Instruction Decoder
 *
 *  The Instruction Decoder extracts bits from the instruction and turns
 *  them into separate fields that go to various functional units in the
 *  processor.
 */


/*!
 * This function dynamically allocates and initializes the state for a new 
 * branching instruction decoder instance.  If allocation fails, the program is
 * terminated.
 */
Decode * build_decode() {
    Decode *d = malloc(sizeof(Decode));
    if (!d) {
        fprintf(stderr, "Out of memory building an instruction decoder!\n");
        exit(11);
    }
    memset(d, 0, sizeof(Decode));
    return d;
}


/*!
 * This function frees the dynamically allocated instruction-decoder instance.
 */
void free_decode(Decode *d) {
    free(d);
}


/*!
 * This function decodes the instruction on the input pin, and writes all of the
 * various components to output pins.  Other components can then read their
 * respective parts of the instruction.
 *
 * NOTE:  the busdata_t type is defined in bus.h, and is a uint32_t
 */
void fetch_and_decode(InstructionStore *is, Decode *d, ProgramCounter *pc) {
    /* This is the current instruction byte we are decoding. */ 
    unsigned char instr_byte;

    /* The CPU operation the instruction represents.  This will be one of the
     * OP_XXXX values from instruction.h.
     */
    busdata_t operation;
    
    /* Source-register values, including defaults for src1-related values. */
    busdata_t src1_addr = 0, src1_const = 0, src1_isreg = 1;
    busdata_t src2_addr = 0;

    /* Destination register.  For both single-argument and two-argument
     * instructions, dst == src2.
     */
    busdata_t dst_addr = 0;
    
    /* Flag controlling whether the destination register is written to.
     * Default value is to *not* write to the destination register.
     */
    busdata_t dst_write = NOWRITE_REG;

    /* The branching address loaded from a branch instruction. */
    busdata_t branch_addr = 0;

    /* All instructions have at least one byte, so read the first byte. */
    ifetch(is); /* Cause InstructionStore to push out the instruction byte */
    instr_byte = pin_read(d->input);

    /* src_mask is a mask used to extract the lower register bits */
    unsigned char src_mask = 0x07;
    /* instr_mask is a mask used to extract the operation bits */
    unsigned char instr_mask = 0xF0;
    /** 
     * addr_mask is a mask that used to extract register address from 
     * multi-byte input.
     */
    unsigned char addr_mask = 0xF;

    /* Temporary variables */
    unsigned char op_type, temp_register;
    
    op_type = (instr_mask & instr_byte);
    op_type >>= 4;

    switch (op_type) {
        case OP_DONE: {
            /* Zero argument instructions. */
            operation = op_type;
            break;
        }
        /* One argument instructions, Cases fall through. */
        case OP_INC:
        case OP_DEC:
        case OP_NEG:
        case OP_INV:
        case OP_SHL:
        case OP_SHR: {
            /* Updates the appropriate pin variables respectively. */
            operation = op_type;
            src1_addr = src_mask & instr_byte;
            src2_addr = src1_addr;
            /* Turn on write flag. */
            dst_write = WRITE_REG;
            break;
        }
        /* Two argument instructions, cases fall through. */
        case OP_MOV:
        case OP_ADD:
        case OP_SUB:
        case OP_AND:
        case OP_OR:
        case OP_XOR:{
            /* Updates the appropriate pin variables respectively. */
            operation = op_type;
            unsigned char is_regis_mask = 0x08;
            /* src2 is always an address */
            src2_addr = src_mask & instr_byte;
            /* Determines whether the second byte is a 
             * register or constant.
             */
            src1_isreg = (instr_byte & is_regis_mask) >> 3;

            /* Reads the next instruction byte. */
            incrPC(pc);
            ifetch(is);
            instr_byte = pin_read(d->input);
            
            /* Writes to address or constant depending on the input. */
            if (src1_isreg) {
                src1_addr = src_mask & instr_byte;
            } else {
                src1_const = instr_byte;
            }
            /* Turn on write flag. */
            dst_write = WRITE_REG;
            break;
        }
        /* Branch Instructions, cases fall through */
        case OP_BRA:
        case OP_BRZ:
        case OP_BNZ: {
            /* Updates the appropriate pin variables respectively. */
            operation = op_type;
            branch_addr = addr_mask & instr_byte;
            break;
        }
        default:
        /* Should never reach here if all instructions are valid. */
        printf("%s\n", "Error in decoding, OP_CODE not found");
        break;

    }


    /* All decoded!  Write out the decoded values. */

    pin_set(d->cpuop,       operation);

    pin_set(d->src1_addr,   src1_addr);
    pin_set(d->src1_const,  src1_const);
    pin_set(d->src1_isreg,  src1_isreg);

    pin_set(d->src2_addr,   src2_addr);

    /* For this processor, like x86-64, dst is always src2. */
    pin_set(d->dst_addr,    src2_addr);
    pin_set(d->dst_write,   dst_write);

    pin_set(d->branch_addr, branch_addr);
}
