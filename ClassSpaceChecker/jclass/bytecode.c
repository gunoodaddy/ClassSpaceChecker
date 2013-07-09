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
 * $Id: bytecode.c 525 2005-01-28 00:44:21Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jclass/bytecode.h>

/* Instruction information. */
struct Instruction {
	char name[16];
	uint8_t no_operands;
	/* Length of operands in bytes. */
	uint8_t operands_length;
	/* Type of the first operand. */
	OperandType operand_type;
};

static struct Instruction InstructionTable[] = {
	{ "nop",			0,	0, OP_TYPE_NONE }, /* 0 */
	{ "aconst_null",	0,	0, OP_TYPE_NONE },
	{ "iconst_m1",		0,	0, OP_TYPE_NONE },
	{ "iconst_0",		0,	0, OP_TYPE_NONE },
	{ "iconst_1",		0,	0, OP_TYPE_NONE },
	{ "iconst_2",		0,	0, OP_TYPE_NONE }, /* 5 */
	{ "iconst_3",		0,	0, OP_TYPE_NONE },
	{ "iconst_4",		0,	0, OP_TYPE_NONE },
	{ "iconst_5",		0,	0, OP_TYPE_NONE },
	{ "lconst_0",		0,	0, OP_TYPE_NONE },
	{ "lconst_1",		0,	0, OP_TYPE_NONE }, /* 10 */
	{ "fconst_0",		0,	0, OP_TYPE_NONE },
	{ "fconst_1",		0,	0, OP_TYPE_NONE },
	{ "fconst_2",		0,	0, OP_TYPE_NONE },
	{ "dconst_0",		0,	0, OP_TYPE_NONE },
	{ "dconst_1",		0,	0, OP_TYPE_NONE }, /* 15 */
	{ "bipush",			1,	1, OP_TYPE_BYTE },
	{ "sipush",			1,	2, OP_TYPE_SHORT },
	{ "ldc",			1,	1, OP_TYPE_BYTE_CONSTANT_INDEX },
	{ "ldc_w",			1,	2, OP_TYPE_SHORT_CONSTANT_INDEX },
	{ "ldc2_w",			1,	2, OP_TYPE_SHORT_CONSTANT_INDEX }, /* 20 */
	{ "iload",			1,	1, OP_TYPE_BYTE },
	{ "lload",			1,	1, OP_TYPE_BYTE },
	{ "fload",			1,	1, OP_TYPE_BYTE },
	{ "dload",			1,	1, OP_TYPE_BYTE },
	{ "aload",			1,	1, OP_TYPE_BYTE }, /* 25 */
	{ "iload_0",		0,	0, OP_TYPE_NONE },
	{ "iload_1",		0,	0, OP_TYPE_NONE },
	{ "iload_2",		0,	0, OP_TYPE_NONE },
	{ "iload_3",		0,	0, OP_TYPE_NONE },
	{ "lload_0",		0,	0, OP_TYPE_NONE }, /* 30 */
	{ "lload_1",		0,	0, OP_TYPE_NONE },
	{ "lload_2",		0,	0, OP_TYPE_NONE },
	{ "lload_3",		0,	0, OP_TYPE_NONE },
	{ "fload_0",		0,	0, OP_TYPE_NONE },
	{ "fload_1",		0,	0, OP_TYPE_NONE }, /* 35 */
	{ "fload_2",		0,	0, OP_TYPE_NONE },
	{ "fload_3",		0,	0, OP_TYPE_NONE },
	{ "dload_0",		0,	0, OP_TYPE_NONE },
	{ "dload_1",		0,	0, OP_TYPE_NONE },
	{ "dload_2",		0,	0, OP_TYPE_NONE }, /* 40 */
	{ "dload_3",		0,	0, OP_TYPE_NONE },
	{ "aload_0",		0,	0, OP_TYPE_NONE },
	{ "aload_1",		0,	0, OP_TYPE_NONE },
	{ "aload_2",		0,	0, OP_TYPE_NONE },
	{ "aload_3",		0,	0, OP_TYPE_NONE }, /* 45 */
	{ "iaload",			0,	0, OP_TYPE_NONE },
	{ "laload",			0,	0, OP_TYPE_NONE },
	{ "faload",			0,	0, OP_TYPE_NONE },
	{ "daload",			0,	0, OP_TYPE_NONE },
	{ "aaload",			0,	0, OP_TYPE_NONE }, /* 50 */
	{ "baload",			0,	0, OP_TYPE_NONE },
	{ "caload",			0,	0, OP_TYPE_NONE },
	{ "saload",			0,	0, OP_TYPE_NONE },
	{ "istore",			1,	1, OP_TYPE_BYTE },
	{ "lstore",			1,	1, OP_TYPE_BYTE }, /* 55 */
	{ "fstore",			1,	1, OP_TYPE_BYTE },
	{ "dstore",			1,	1, OP_TYPE_BYTE },
	{ "astore",			1,	1, OP_TYPE_BYTE },
	{ "istore_0",		0,	0, OP_TYPE_NONE },
	{ "istore_1",		0,	0, OP_TYPE_NONE }, /* 60 */
	{ "istore_2",		0,	0, OP_TYPE_NONE },
	{ "istore_3",		0,	0, OP_TYPE_NONE },
	{ "lstore_0",		0,	0, OP_TYPE_NONE },
	{ "lstore_1",		0,	0, OP_TYPE_NONE },
	{ "lstore_2",		0,	0, OP_TYPE_NONE }, /* 65 */
	{ "lstore_3",		0,	0, OP_TYPE_NONE },
	{ "fstore_0",		0,	0, OP_TYPE_NONE },
	{ "fstore_1",		0,	0, OP_TYPE_NONE },
	{ "fstore_2",		0,	0, OP_TYPE_NONE },
	{ "fstore_3",		0,	0, OP_TYPE_NONE }, /* 70 */
	{ "dstore_0",		0,	0, OP_TYPE_NONE },
	{ "dstore_1",		0,	0, OP_TYPE_NONE },
	{ "dstore_2",		0,	0, OP_TYPE_NONE },
	{ "dstore_3",		0,	0, OP_TYPE_NONE },
	{ "astore_0",		0,	0, OP_TYPE_NONE }, /* 75 */
	{ "astore_1",		0,	0, OP_TYPE_NONE },
	{ "astore_2",		0,	0, OP_TYPE_NONE },
	{ "astore_3",		0,	0, OP_TYPE_NONE },
	{ "iastore",		0,	0, OP_TYPE_NONE },
	{ "lastore",		0,	0, OP_TYPE_NONE }, /* 80 */
	{ "fastore",		0,	0, OP_TYPE_NONE },
	{ "dastore",		0,	0, OP_TYPE_NONE },
	{ "aastore",		0,	0, OP_TYPE_NONE },
	{ "bastore",		0,	0, OP_TYPE_NONE },
	{ "castore",		0,	0, OP_TYPE_NONE }, /* 85 */
	{ "sastore",		0,	0, OP_TYPE_NONE },
	{ "pop",			0,	0, OP_TYPE_NONE },
	{ "pop2",			0,	0, OP_TYPE_NONE },
	{ "dup",			0,	0, OP_TYPE_NONE },
	{ "dup_x1",			0,	0, OP_TYPE_NONE }, /* 90 */
	{ "dup_x2",			0,	0, OP_TYPE_NONE },
	{ "dup2",			0,	0, OP_TYPE_NONE },
	{ "dup2_x1",		0,	0, OP_TYPE_NONE },
	{ "dup2_x2",		0,	0, OP_TYPE_NONE },
	{ "swap",			0,	0, OP_TYPE_NONE }, /* 95 */
	{ "iadd",			0,	0, OP_TYPE_NONE },
	{ "ladd",			0,	0, OP_TYPE_NONE },
	{ "fadd",			0,	0, OP_TYPE_NONE },
	{ "dadd",			0,	0, OP_TYPE_NONE },
	{ "isub",			0,	0, OP_TYPE_NONE }, /* 100 */
	{ "lsub",			0,	0, OP_TYPE_NONE },
	{ "fsub",			0,	0, OP_TYPE_NONE },
	{ "dsub",			0,	0, OP_TYPE_NONE },
	{ "imul",			0,	0, OP_TYPE_NONE },
	{ "lmul",			0,	0, OP_TYPE_NONE }, /* 105 */
	{ "fmul",			0,	0, OP_TYPE_NONE },
	{ "dmul",			0,	0, OP_TYPE_NONE },
	{ "idiv",			0,	0, OP_TYPE_NONE },
	{ "ldiv",			0,	0, OP_TYPE_NONE },
	{ "fdiv",			0,	0, OP_TYPE_NONE }, /* 110 */
	{ "ddiv",			0,	0, OP_TYPE_NONE },
	{ "irem",			0,	0, OP_TYPE_NONE },
	{ "lrem",			0,	0, OP_TYPE_NONE },
	{ "frem",			0,	0, OP_TYPE_NONE },
	{ "drem",			0,	0, OP_TYPE_NONE }, /* 115 */
	{ "ineg",			0,	0, OP_TYPE_NONE },
	{ "lneg",			0,	0, OP_TYPE_NONE },
	{ "fneg",			0,	0, OP_TYPE_NONE },
	{ "dneg",			0,	0, OP_TYPE_NONE },
	{ "ishl",			0,	0, OP_TYPE_NONE }, /* 120 */
	{ "lshl",			0,	0, OP_TYPE_NONE },
	{ "ishr",			0,	0, OP_TYPE_NONE },
	{ "lshr",			0,	0, OP_TYPE_NONE },
	{ "iushr",			0,	0, OP_TYPE_NONE },
	{ "lushr",			0,	0, OP_TYPE_NONE }, /* 125 */
	{ "iand",			0,	0, OP_TYPE_NONE },
	{ "land",			0,	0, OP_TYPE_NONE },
	{ "ior",			0,	0, OP_TYPE_NONE },
	{ "lor",			0,	0, OP_TYPE_NONE },
	{ "ixor",			0,	0, OP_TYPE_NONE }, /* 130 */
	{ "lxor",			0,	0, OP_TYPE_NONE },
	{ "iinc",			2,	2, OP_TYPE_BYTE },
	{ "i2l",			0,	0, OP_TYPE_NONE },
	{ "i2f",			0,	0, OP_TYPE_NONE },
	{ "i2d",			0,	0, OP_TYPE_NONE }, /* 135 */
	{ "l2i",			0,	0, OP_TYPE_NONE },
	{ "l2f",			0,	0, OP_TYPE_NONE },
	{ "l2d",			0,	0, OP_TYPE_NONE },
	{ "f2i",			0,	0, OP_TYPE_NONE },
	{ "f2l",			0,	0, OP_TYPE_NONE }, /* 140 */
	{ "f2d",			0,	0, OP_TYPE_NONE },
	{ "d2i",			0,	0, OP_TYPE_NONE },
	{ "d2l",			0,	0, OP_TYPE_NONE },
	{ "d2f",			0,	0, OP_TYPE_NONE },
	{ "i2b",			0,	0, OP_TYPE_NONE }, /* 145 */
	{ "i2c",			0,	0, OP_TYPE_NONE },
	{ "i2s",			0,	0, OP_TYPE_NONE },
	{ "lcmp",			0,	0, OP_TYPE_NONE },
	{ "fcmpl",			0,	0, OP_TYPE_NONE },
	{ "fcmpg",			0,	0, OP_TYPE_NONE }, /* 150 */
	{ "dcmpl",			0,	0, OP_TYPE_NONE },
	{ "dcmpg",			0,	0, OP_TYPE_NONE },
	{ "ifeq",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "ifne",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "iflt",			1,	2, OP_TYPE_SHORT_OFFSET }, /* 155 */
	{ "ifge",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "ifgt",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "ifle",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "if_icmpeq",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "if_icmpne",		1,	2, OP_TYPE_SHORT_OFFSET }, /* 160 */
	{ "if_icmplt",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "if_icmpge",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "if_icmpgt",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "if_icmple",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "if_acmpeq",		1,	2, OP_TYPE_SHORT_OFFSET }, /* 165 */
	{ "if_acmpne",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "goto",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "jsr",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "ret",			1,	1, OP_TYPE_BYTE },
	{ "tableswitch",	1, OP_LENGTH_UNPREDICTABLE, OP_TYPE_TABLESWITCH },	/* 170 */
	{ "lookupswitch",	1, OP_LENGTH_UNPREDICTABLE, OP_TYPE_LOOKUPSWITCH },
	{ "ireturn",		0,	0, OP_TYPE_NONE },
	{ "lreturn",		0,	0, OP_TYPE_NONE },
	{ "freturn",		0,	0, OP_TYPE_NONE },
	{ "dreturn",		0,	0, OP_TYPE_NONE }, /* 175 */
	{ "areturn",		0,	0, OP_TYPE_NONE },
	{ "return",			0,	0, OP_TYPE_NONE },
	{ "getstatic",		1,	2, OP_TYPE_SHORT_FIELD_INDEX },
	{ "putstatic",		1,	2, OP_TYPE_SHORT_FIELD_INDEX },
	{ "getfield",		1,	2, OP_TYPE_SHORT_FIELD_INDEX }, /* 180 */
	{ "putfield",		1,	2, OP_TYPE_SHORT_FIELD_INDEX },
	{ "invokevirtual",	1,	2, OP_TYPE_SHORT_METHOD_INDEX },
	{ "invokespecial",	1,	2, OP_TYPE_SHORT_METHOD_INDEX },
	{ "invokestatic",	1,	2, OP_TYPE_SHORT_METHOD_INDEX },
	{ "invokeinterface", 3, 4, OP_TYPE_SHORT_METHOD_INDEX }, /* 185 */
	{ "xxxunusedxxx1",	0,	0, OP_TYPE_NONE },
	{ "new",			1,	2, OP_TYPE_SHORT_CLASS_INDEX },
	{ "newarray",		1,	1, OP_TYPE_BYTE_ARRAY_TYPE },
	{ "anewarray",		1,	2, OP_TYPE_SHORT_CLASS_INDEX },
	{ "arraylength",	0,	0, OP_TYPE_NONE }, /* 190 */
	{ "athrow",			0,	0, OP_TYPE_NONE },
	{ "checkcast",		1,	2, OP_TYPE_SHORT_CLASS_INDEX },
	{ "instanceof",		1,	2, OP_TYPE_SHORT_CLASS_INDEX },
	{ "monitorenter",	0,	0, OP_TYPE_NONE },
	{ "monitorexit",	0,	0, OP_TYPE_NONE }, /* 195 */
	/* Widens the range of the instruction after it.
	* Opcode: 0xC4 (196)
	* For: aload, dload, iload, fload, lload, astore, dstore, istore, fstore, lstore, and ret\n
	* wide <uint8 opcode> <u2 param>
	* For: iinc\n
	* wide <uint8 opcode> <u2 param> <u2 n>
	*/
	{ "wide",			1,	OP_LENGTH_UNPREDICTABLE, OP_TYPE_INSTRUCTION},
	{ "multianewarray", 2,	3, OP_TYPE_SHORT_CLASS_INDEX},
	{ "ifnull",			1,	2, OP_TYPE_SHORT_OFFSET },
	{ "ifnonnull",		1,	2, OP_TYPE_SHORT_OFFSET },
	{ "goto_w",			1,	4, OP_TYPE_INT_OFFSET },	/* 200 */
	{ "jsr_w",			1,	4, OP_TYPE_INT_OFFSET }
};

/**
* jclass_code_array_name
* @array_number: The OP_TYPE_BYTE_ARRAY_TYPE operand.
*
* Gives the name of the array type given a OP_TYPE_BYTE_ARRAY_TYPE operand.
*
* Returns: A constant string you should not modify.
*/
const char* jclass_code_array_name(uint8_t array_number)
{
	static char* array_type[] = {
		"boolean",
		"char",
		"float",
		"double",
		"byte",
		"short",
		"int",
		"long"
	};

	if(array_number > 3 && array_number < 12)
		return array_type[array_number - 4];
	else
		return NULL;
}

/**
* jclass_code_instruction_name
* @opcode: The opcode for the instruction.
*
* Gives the name of the instruction with the given opcode.
* The string returned is from an internal table so don't try to free it.
* If the opcode is illegal it returns NULL.
*
* Returns: A constant string with the mnemonic for the instruction.
*/
const char* jclass_code_instruction_name(uint8_t opcode)
{
	if(opcode > MAX_LEGAL_OPCODE)
		return NULL;
	else
		return InstructionTable[opcode].name;
}

/**
* jclass_code_instruction_ops
* @opcode: The opcode for the instruction.
*
* Gives the number of the operands for the instruction.
* If the opcode is illegal it returns 0.
*
* Returns: The number of operands.
*/
uint8_t jclass_code_instruction_ops(uint8_t opcode)
{
	if(opcode > MAX_LEGAL_OPCODE)
		return 0;
	else
		return InstructionTable[opcode].no_operands;
}

/**
* jclass_code_instruction_ops_length
* @opcode: The opcode for the instruction.
*
* Gives the length of the operands for the instruction in bytes.
* If the instruction is widened multiply this number by 2. 
* If the opcode is illegal it returns 0.
* If the length cannot be predicted it returns OP_LENGTH_UNPREDICTABLE.
*
* Returns: The length of the operands in bytes.
*/
uint8_t jclass_code_instruction_ops_length(uint8_t opcode)
{
	if(opcode > MAX_LEGAL_OPCODE)
		return 0;
	else
		return InstructionTable[opcode].operands_length;
}

/**
* jclass_code_instruction_op_type
* @opcode: The opcode for the instruction.
* @operand_number: The number of the operand (starting from 0) to get its type. 
* @is_wide: Set to 1 if the instruction is widened, 0 otherwise.
*
* Gives the type of the given operand.
* If an inconsistency is detected such as asking for the wide form of an
* instruction that does not have one it returns OP_TYPE_ERROR.
*
* Returns: An OperandType enum for the type of the operand.
*/
OperandType jclass_code_instruction_op_type(uint8_t opcode, int operand_number, int is_wide)
{
	if(opcode > MAX_LEGAL_OPCODE)
		return OP_TYPE_ERROR;

	if(InstructionTable[opcode].no_operands <= operand_number)
		return OP_TYPE_NONE;

	switch(operand_number)
	{
		case 0:
			if(is_wide)
			{
				switch(opcode)
				{
					case 21: /* iload */
					case 22: /* lload */
					case 23: /* fload */
					case 24: /* dload */
					case 25: /* aload */
					case 54: /* istore */
					case 55: /* lstore */
					case 56: /* fstore */
					case 57: /* dstore */
					case 83: /* astore */
					case 132: /* iinc */
					case 169: /* ret */
						return OP_TYPE_UNSIGNED_SHORT;
					default:
						return OP_TYPE_ERROR;
				}
			}
			else
				return InstructionTable[opcode].operand_type;
			break;
		case 1:
			switch(opcode)
			{
				case 132: /* iinc */
					if(is_wide)
						return OP_TYPE_UNSIGNED_SHORT;
					else
						return OP_TYPE_BYTE;
				case 185: /* invokeinterface */
					return OP_TYPE_UNSIGNED_BYTE_USELESS;
				case 197: /* multianewarray */
					return OP_TYPE_UNSIGNED_BYTE;
				default:
					return OP_TYPE_ERROR;
			}
			break;
		case 2:
			/* invokeinterface */
			return OP_TYPE_UNSIGNED_BYTE_USELESS;
		default:
			return OP_TYPE_ERROR;
	}
}

/**
* jclass_code_read_byte
* @code: The code array.
* @pc: A pointer to the PC counter.
*
* Reads a signed byte operand and increments the pc by the length of the operand.
*
* Returns: The signed byte at the current pc.
*/
int8_t jclass_code_read_byte(const uint8_t* code, uint32_t* pc)
{
	int8_t s1op;

	s1op = code[*pc];
	(*pc)++;
	return s1op;
}

/**
* jclass_code_read_ubyte
* @code: The code array.
* @pc: A pointer to the PC counter.
*
* Reads an unsigned byte operand and increments the pc by the length of the operand.
*
* Returns: The unsigned byte at the current pc.
*/
uint8_t jclass_code_read_ubyte(const uint8_t* code, uint32_t* pc)
{
	uint8_t u1op;

	u1op = code[*pc];
	(*pc)++;
	return u1op;
}

/**
* jclass_code_read_short
* @code: The code array.
* @pc: A pointer to the PC counter.
*
* Reads a short operand and increments the pc by the length of the operand.
*
* Returns: The short at the current pc.
*/
int16_t jclass_code_read_short(const uint8_t* code, uint32_t* pc)
{
	int16_t s2op;

	memcpy(&s2op, &code[*pc], 2);
	s2op = INT16_NATIVE(s2op);
	(*pc) += 2;
	return s2op;
}

/**
* jclass_code_read_ushort
* @code: The code array.
* @pc: A pointer to the PC.
*
* Reads an unsigned short operand and increments the pc by the length of the operand.
*
* Returns: The unsigned short at the current pc.
*/
uint16_t jclass_code_read_ushort(const uint8_t* code, uint32_t* pc)
{
	uint16_t u2op;

	memcpy(&u2op, &code[*pc], 2);
	u2op = UINT16_NATIVE(u2op);
	(*pc) += 2;
	return u2op;
}

/**
* jclass_code_read_int
* @code: The code array.
* @pc: A pointer to the program counter.
*
* Reads an integer operand and increments the pc by the length of the operand.
*
* Returns: The integer at the current pc.
*/
int32_t jclass_code_read_int(const uint8_t* code, uint32_t* pc)
{
	int32_t s4op;

	memcpy(&s4op, &code[*pc], 4);
	s4op = INT32_NATIVE(s4op);
	(*pc) += 4;
	return s4op;
}

/**
* jclass_code_read_tableswitch
* @code: The code array.
* @pc: A pointer to the PC.
*
* Reads a tableswitch operand and increments the pc by the length of the operand.
*
* Returns: A TableSwitch structure allocated with malloc.
*/
TableSwitchOperand* jclass_code_read_tableswitch(const uint8_t* code, uint32_t* pc)
{
	TableSwitchOperand* operand;
	uint32_t instruction_pc;
	int32_t count;

	instruction_pc = (*pc) - 1;
	*pc = JCLASS_CODE_ALIGN_PC(*pc);
	operand = (TableSwitchOperand*) malloc(sizeof(TableSwitchOperand));

	operand->default_target = instruction_pc + jclass_code_read_int(code, pc);
	operand->low_value = jclass_code_read_int(code, pc);
	operand->num_pairs = (jclass_code_read_int(code, pc) - operand->low_value) + 1;
	operand->target = (uint32_t*) malloc(sizeof(uint32_t) * operand->num_pairs);

	for(count = 0; count < operand->num_pairs; count++)
		operand->target[count] = instruction_pc + jclass_code_read_int(code, pc);

	return operand;
}

/**
* jclass_code_tableswitch_operand_free
* @operand: The TableSwitch operand to free.
*
* Frees a TableSwitch operand.
*/
void jclass_code_tableswitch_operand_free(TableSwitchOperand* operand)
{
	free(operand->target);
	free(operand);
}

/**
* jclass_code_read_lookupswitch
* @code: The code array.
* @pc: A pointer to the PC.
*
* Reads a lookupswitch operand and increments the pc by the length of the operand.
*
* Returns: A newly created LookupSwitch structure.
*/
LookupSwitchOperand* jclass_code_read_lookupswitch(const uint8_t* code, uint32_t* pc)
{
	LookupSwitchOperand* operand;
	uint32_t instruction_pc;
	int32_t count;

	instruction_pc = (*pc) - 1;
	*pc = JCLASS_CODE_ALIGN_PC(*pc);
	operand = (LookupSwitchOperand*) malloc(sizeof(LookupSwitchOperand));

	operand->default_target = instruction_pc + jclass_code_read_int(code, pc);
	operand->num_pairs = jclass_code_read_int(code, pc);
	operand->value = (uint32_t*) malloc(sizeof(uint32_t) * operand->num_pairs);
	operand->target = (uint32_t*) malloc(sizeof(uint32_t) * operand->num_pairs);
	for(count = 0; count < operand->num_pairs; count++)
	{
		operand->value[count] = instruction_pc + jclass_code_read_int(code, pc);
		operand->target[count] = instruction_pc + jclass_code_read_int(code, pc);
    }
	return operand;
}

/**
* jclass_code_lookupswitch_operand_free
* @operand: The LookupSwitch operand to free.
*
* Frees a LookupSwitch operand.
*/
void jclass_code_lookupswitch_operand_free(LookupSwitchOperand* operand)
{
	free(operand->value);
	free(operand->target);
	free(operand);
}
