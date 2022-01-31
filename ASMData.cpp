#include <iostream>
#include <string.h>
#include <stdint.h>
#include "ASMData.hpp"
#include "VMModel.hpp"

#define NUMREGISTERS 34
#define NUMINSTRUCTIONS 42

static ASMData Registers[NUMREGISTERS] = {
	{"$zero", "$0"},{"$at", "$1"},{"$v0", "$2"},{ "$v1", "$3" },
	{ "$a0", "$4" },{ "$a1", "$5" },{ "$a2", "$6" },{ "$a3", "$7" },
	{ "$t0", "$8" },{ "$t1", "$9" },{ "$t2", "$10" },{ "$t3", "$11" },
	{ "$t4", "$12" },{ "$t5", "$13" },{ "$t6", "$14" },{ "$t7", "$15" },
	{ "$s0", "$16" },{ "$s1", "$17" },{ "$s2", "$18" },{ "$s3", "$19" },
	{ "$s4", "$20" },{ "$s5", "$21" },{ "$s6", "$22" },{ "$s7", "$23" },
	{ "$t8", "$24" },{ "$t9", "$25" },{ "$k0", "$26" },{ "$k1", "$27" },
	{ "$gp", "$28" },{ "$sp", "$29" },{ "$fp", "$30" },{ "$ra", "$31" },
	{"$hi", ""}, {"$lo", ""}
};

static OPCode OPTable[NUMINSTRUCTIONS] = {
	//Data movement instructions
	{"lw", "r,m"},{"lh", "r,m"},{"lb", "r,m"},{"li", "r,i"},
	{"la", "r,m"},{"sw", "r,m"},{"sh", "r,m"},{"sb", "r,m"},
	{"move", "r,r"},{"mfhi", "r"},{"mflo", "r"},{"mthi", "r"},
	{"mtlo", "r"},

	//Integer Arithmetic instructions
	{"add", "r,r,s"},{"addu", "r,r,s"},{"sub", "r,r,s"},{"subu", "r,r,s"},
	{"mul", "r,r,s"},{"mulo", "r,r,s"},{"mulou", "r,r,s"},{"mult", "r,r"},
	{"multu", "r,r"},{"div", "*"},{"divu", "*"},{"rem", "r,r,s"},
	{"remu", "r,r,s"},{"abs", "r,r"},{"neg", "r,r"},{"negu", "r,r"},

	//Logical instructions
	{"and", "r,r,s"},{"nor", "r,r,s"},{"not", "r,s"},{"or", "r,r,s"},
	{"xor", "r,r,s"},

	//Control instructions
	{"j", "l"},{"beq", "r,s,l"},{"bne", "r,s,l"},{"blt", "r,s,l"},
	{"ble", "r,s,l"},{"bgt", "r,s,l"},{"bge", "r,s,l"},

	//nop
	{"nop", ""}
};


const ASMData findReg(const string name)
{
	for (int i = 0; i < NUMREGISTERS; i++)
		if (name == Registers[i].getName() || name == Registers[i].getNumber())
			return Registers[i];
	return { "", "" };	//opcode was not found
}

const OPCode findOP(const string name)
{
	for (int i = 0; i < NUMINSTRUCTIONS; i++)
		if (strcmp(name.c_str(), OPTable[i].getName().c_str()) == 0)
			return OPTable[i];
	return { "", "" };	//opcode was not found
}

void initRegisters(unordered_map<string, uint32_t> &registers)
{
	for (int i = 0; i < NUMREGISTERS; i++)
	{
		registers[Registers[i].getName()] = 0;
	}
}