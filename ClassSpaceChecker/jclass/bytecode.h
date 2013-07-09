/* libjclass - Library for reading java class files
 * Copyright (C) 2003-2004 Nicos Panayides
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 * $Id: bytecode.h 525 2005-01-28 00:44:21Z anarxia $
 */

#ifndef __JCLASS_BYTECODE_H__
#define __JCLASS_BYTECODE_H__

#ifdef _cplusplus
  extern "C" {
#endif 

#include <jclass/types.h>
	  
#define OP_LENGTH_UNPREDICTABLE	0x10

typedef enum {
	OP_TYPE_NONE,
	OP_TYPE_BYTE,
	OP_TYPE_UNSIGNED_BYTE,
	OP_TYPE_BYTE_ARRAY_TYPE,
	OP_TYPE_BYTE_CONSTANT_INDEX,
	OP_TYPE_UNSIGNED_BYTE_USELESS,
	OP_TYPE_SHORT,
	OP_TYPE_UNSIGNED_SHORT,
	OP_TYPE_SHORT_OFFSET,
	OP_TYPE_SHORT_CLASS_INDEX,
	OP_TYPE_SHORT_METHOD_INDEX,
	OP_TYPE_SHORT_FIELD_INDEX,
	OP_TYPE_SHORT_CONSTANT_INDEX,
	OP_TYPE_INT,
	OP_TYPE_INT_OFFSET,
	OP_TYPE_INSTRUCTION,
	OP_TYPE_TABLESWITCH,
	OP_TYPE_LOOKUPSWITCH,
	OP_TYPE_ERROR
} OperandType;

typedef enum {
	ARRAY_TYPE_BOOLEAN = 4,
	ARRAY_TYPE_CHAR,
	ARRAY_TYPE_FLOAT,
	ARRAY_TYPE_DOUBLE,
	ARRAY_TYPE_BYTE,
	ARRAY_TYPE_SHORT,
	ARRAY_TYPE_INT,
	ARRAY_TYPE_LONG
} ArrayType;

typedef struct {
	int32_t num_pairs;
	uint32_t default_target;
	int32_t low_value;
	uint32_t* target;
} TableSwitchOperand;

typedef struct {
	int32_t num_pairs;
	uint32_t default_target;
	uint32_t* value;
	uint32_t* target;
} LookupSwitchOperand;

#define MAX_LEGAL_OPCODE 201

const char* jclass_code_instruction_name(uint8_t opcode);
uint8_t jclass_code_instruction_ops(uint8_t opcode);
uint8_t jclass_code_instruction_ops_length(uint8_t opcode);
OperandType jclass_code_instruction_op_type(uint8_t opcode,
											int operand_number,
											int is_wide);

int8_t jclass_code_read_byte(const uint8_t* code, uint32_t* pc);
uint8_t jclass_code_read_ubyte(const uint8_t* code, uint32_t* pc);
int16_t jclass_code_read_short(const uint8_t* code, uint32_t* pc);
uint16_t jclass_code_read_ushort(const uint8_t* code, uint32_t* pc);
int32_t jclass_code_read_int(const uint8_t* code, uint32_t* pc);
TableSwitchOperand* jclass_code_read_tableswitch(const uint8_t* code, uint32_t* pc);
LookupSwitchOperand* jclass_code_read_lookupswitch(const uint8_t* code, uint32_t* pc);
void jclass_code_lookupswitch_operand_free(LookupSwitchOperand* operand);
void jclass_code_tableswitch_operand_free(TableSwitchOperand* operand);

#define JCLASS_CODE_ALIGN_PC(pc) (((pc) % 4) ? (((pc) + 4) - ((pc) % 4)) : (pc))

const char* jclass_code_array_name(uint8_t array_number);

#ifdef _cplusplus
  }
#endif 

#endif
