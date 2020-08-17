#pragma once

// Implementation of lc3 compliant virtual machine
// Following this tutorial:
// https://justinmeiners.github.io/lc3-vm/
//

#include <cstdint>
#include <binstream.hpp>

namespace lc3vm
{
	// Condition flags
	enum {
		FL_POS = 1 << 0,	// P
		FL_ZRO = 1 << 1,	// Z
		FL_NEG = 1 << 2,	// N
	};

	// Register names
	enum {
		R_R0 = 0,
		R_R1,
		R_R2,
		R_R3,
		R_R4,
		R_R5,
		R_R6,
		R_R7,
		R_PC,	// Program Counter
		R_COND,		// Conditional flags
		R_COUNT,	// How many registers
	};



	// Instruction Set
	enum {
		OP_BR = 0,		// branch
		OP_ADD,			// add
		OP_LD,			// load
		OP_ST,			// store
		OP_JSR,			// jump register
		OP_AND,			// bitwise and
		OP_LDR,			// load register
		OP_STR,			// store register
		OP_RTI,			// unused
		OP_NOT,			// bitwise not
		OP_LDI,			// load indirect
		OP_STI,			// store indirect
		OP_JMP,			// jump
		OP_RES,			// reserved (unused)
		OP_LEA,			// load effective address
		OP_TRAP,		// execute trap
	};

	// Memory Mapped Registers
	enum {
		MR_KBSR = 0xFE00,	// keyboard status
		MR_KBDR = 0xFE02,	// keyboard data
	};

	// Trap codes
	enum {
		TRAP_GETC	= 0x20,		// get character from keyboard, not echoed onto the terminal
		TRAP_OUT	= 0x21,		// output a character
		TRAP_PUTS	= 0x22,		// output a word string
		TRAP_IN		= 0x23,		// get character from keyboard, echoed onto the terminal
		TRAP_PUTSP	= 0x24,		// output a byte string
		TRAP_HALT	= 0x25,		// Halt the program
	};

	// Storage
	uint16_t memory[UINT16_MAX];		// Storage for general memory
	uint16_t reg[R_COUNT];				// Storage for registers

	// Execution
	bool running = true;

	uint16_t sign_extend(uint16_t x, int bit_count)
	{
		if ((x >> (bit_count - 1)) & 1) {
			x |= (0xffff << bit_count);
		}

		return x;
	}

	// Swap - swap 16-bit value 
	uint16_t swap16(uint16_t x)
	{
		return (x << 8) | (x >> 8);
	}

	void update_flags(uint16_t r)
	{
		if (reg[r] == 0) {
			reg[R_COND] = FL_ZRO;
		}
		else if (reg[r] >> 15)	// a 1 in most significant bit indicates negative
		{
			reg[R_COND] = FL_NEG;
		}
		else {
			reg[R_COND] = FL_POS;
		}
	}

	// check for a key
	uint16_t check_key()
	{
		return 0;
	}

	uint16_t getchar()
	{
		return 0;
	}

	// Memory Access
	void mem_write(uint16_t address, uint16_t val)
	{
		// Don't worry about bounds checking because the address can
		// only be within valid memory range
		memory[address] = val;
	}

	// Reading a value from memory
	// special cases for memory mapped IO
	uint16_t mem_read(uint16_t address)
	{
		if (address == MR_KBSR) {
			if (check_key()) {
				memory[MR_KBSR] = (1 << 15);
				memory[MR_KBDR] = getchar();
			}
			else {
				memory[MR_KBSR] = 0;
			}
		}

		return memory[address];
	}


	uint16_t read_image_stream(BinStream& bs)
	{
		// read origin from stream
		uint16_t origin = bs.readUInt16();

		// read the rest of the instructions into memory
		// starting at the indicated origin
		uint16_t offset = origin;
		// BUGBUG - should check for offset overflow as well
		while (!bs.isEOF()) {
			memory[offset] = bs.readUInt16();
			offset = offset + 1;
		}

		return origin;
	}

	// Instruction dispatch table
	template <unsigned int op>
	void ins(uint16_t instr) {
		uint16_t r0, r1, r2, imm5, imm_flag;
		uint16_t pc_plus_off, base_plus_off;

		constexpr uint16_t opbit = (1 << op);	// one bit indicating the operator

		if (0x4EEE & opbit) { r0 = (instr >> 9) & 0x7; }
		if (0x12F3 & opbit) { r1 = (instr >> 6) & 0x7; }
		if (0x0022 & opbit) {
			imm_flag = (instr >> 5) & 0x1;
			if (imm_flag != 0) {
				imm5 = sign_extend(instr & 0x1F, 5);
			}
			else
			{
				r2 = instr & 0x7;
			}
		}

		if (0x00C0 & opbit) {
			// base + offset
			base_plus_off = reg[r1] + sign_extend(instr & 0x3F, 6);
		}

		if (0x4C0D & opbit) {
			// indirect address
			pc_plus_off = reg[R_PC] + sign_extend(instr & 0x1FF, 9);
		}

		// BR
		if (0x0001 & opbit) {
			uint16_t cond = (instr >> 9) & 0x07;
			if (cond & reg[R_COND]) { reg[R_PC] = pc_plus_off; }
		}

		// ADD
		if (0x0002 & opbit) {

			if (imm_flag) {
				reg[r0] = reg[r1] + imm5;
			}
			else {
				reg[r0] = reg[r1] + reg[r2];
			}
		}

		// AND
		if (0x0020 & opbit) {
			if (imm_flag) {
				reg[r0] = reg[r1] + imm5;
			}
			else {
				reg[r0] = reg[r1] & reg[r2];
			}
		}

		// NOT
		if (0x0200 & opbit) { reg[r0] = ~reg[r1]; }
	
		// JMP
		if (0x1000 & opbit) { reg[R_PC] = reg[r1]; }

		// JSR
		if (0x0010 & opbit) {
			uint16_t long_flag = (instr >> 11) & 1;
			reg[R_R7] = reg[R_PC];
			if (long_flag) {
				pc_plus_off = reg[R_PC] + sign_extend(instr & 0x7FF, 11);
				reg[R_PC] = pc_plus_off;
			}
			else {
				reg[R_PC] = reg[r1];
			}
		}

		// LD
		if (0x0004 & opbit) { reg[r0] = mem_read(pc_plus_off); }

		// LDI
		if (0x0400 & opbit) { reg[r0] = mem_read(mem_read(pc_plus_off)); }

		// LDR
		if (0x0040 & opbit) { reg[r0] = mem_read(base_plus_off); }

		// LEA
		if (0x4000 & opbit) { reg[r0] = pc_plus_off; }

		// ST
		if (0x0008 & opbit) { mem_write(pc_plus_off, reg[r0]); }

		// STI
		if (0x0800 & opbit) { mem_write(mem_read(pc_plus_off), reg[r0]); }

		// STR
		if (0x0080 & opbit) { mem_write(base_plus_off, reg[r0]); }

		// TRAP
		if (0x8000 & opbit) {
			// BUGBUG - fill this out
		}

		// RTI

		if (0x4666 & opbit) { update_flags(r0); }
	}

	// Operator table
	static void (*op_table[16])(uint16_t) = {
		ins<0>,ins<1>,ins<2>,ins<3>,
		ins<4>,ins<5>,ins<6>,ins<7>,
		NULL,ins<9>,ins<10>,ins<11>,
		ins<12>,NULL,ins<14>,ins<15>
	};

	void run(BinStream& bs)
	{
		// Read the program into memory
		// should return the PC_START
		//enum {PC_START = 0x3000};
		uint16_t PC_START = read_image_stream(bs);
		reg[R_PC] = PC_START;

		// main loop, run until error, or end of program
		bool running = true;
		
		while (running)
		{
			// Fetch
			uint16_t instr = mem_read(reg[R_PC]++);
			uint16_t op = instr >> 12;

			op_table[op](instr);
		}

		// shutdown
	}
}


/*
			// Fetch
			uint16_t instr = mem_read(reg[R_PC]++);
			uint16_t op = instr >> 12;

			switch (op)
			{
			case OP_ADD:
				break;

			case OP_AND:
				//
				break;

			case OP_NOT:
				break;

			case OP_BR:
				break;

			case OP_JMP:
				break;

			case OP_JSR:
				break;

			case OP_LD:
				break;

			case OP_LDI:
				break;

			case OP_LDR:
				break;

			case OP_LEA:
				break;

			case OP_ST:
				break;

			case OP_STI:
				break;

			case OP_STR:
				break;

			case OP_TRAP:
				break;

			case OP_RES:
			case OP_RTI:
			default:
				// bad opcode
				break;
			}
*/