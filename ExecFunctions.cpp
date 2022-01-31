#include <inttypes.h>
#include "VMModel.hpp"
#include "ASMData.hpp"
//#include "VMController.hpp"

/*int VMModel::nop(Instruction instr)
{
	return -1;
}*/


//Data Movement
int VMModel::lw(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	uint32_t offset = atoi(instr.args[1].c_str());
	if ((offset || instr.args[1][0] == '0') && instr.args[1].find("(") >= instr.args[1].size())
	{
		regValues[reg1.getName()] = offset;
		return -1;
	}
	int address = 0;
	int vmIndex = 0;
	if (instr.args[1][instr.args[1].size() - 1] != ')')
	{
		address = dataLabelHelper(instr.args[1]);
		if (address == -1)
		{
			ASMData reg2 = findReg(instr.args[1]);
			vmIndex = regValues[reg2.getName()];
			//regValues[reg1.getName()] = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	else
	{
		int begin = instr.args[1].find("(") + 1;
		int end = instr.args[1].size()-1;
		string memref = instr.args[1].substr(begin, end - begin);
		address = dataLabelHelper(memref);
		if (address == -1)
		{
			ASMData reg2 = findReg(memref);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	regValues[reg1.getName()] = (virtualMem[vmIndex + offset] & 0xff) +
		((virtualMem[vmIndex + 1 + offset] & 0xff) << 8) + ((virtualMem[vmIndex + 2 + offset] & 0xff) << 16) +
		((virtualMem[vmIndex + 3 + offset] & 0xff) << 24);
	return -1;
}

int VMModel::lh(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	uint32_t offset = atoi(instr.args[1].c_str());
	if ((offset || instr.args[1][0] == '0') && instr.args[1].find("(") >= instr.args[1].size())
	{
		regValues[reg1.getName()] = offset;
		return -1;
	}
	int address = 0;
	int vmIndex = 0;
	if (instr.args[1][instr.args[1].size() - 1] != ')')
	{
		address = dataLabelHelper(instr.args[1]);
		if (address == -1)
		{
			ASMData reg2 = findReg(instr.args[1]);
			vmIndex = regValues[reg2.getName()];
			//regValues[reg1.getName()] = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	else
	{
		int begin = instr.args[1].find("(") + 1;
		int end = instr.args[1].size() - 1;
		string memref = instr.args[1].substr(begin, end - begin);
		address = dataLabelHelper(memref);
		if (address == -1)
		{
			ASMData reg2 = findReg(memref);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	regValues[reg1.getName()] = (virtualMem[vmIndex + offset] & 0xff) +
		((virtualMem[vmIndex + 1 + offset] & 0xff) << 8);
	return -1;
}

int VMModel::lb(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	uint32_t offset = atoi(instr.args[1].c_str());
	if ((offset || instr.args[1][0] == '0') && instr.args[1].find("(") >= instr.args[1].size())
	{
		regValues[reg1.getName()] = offset;
		return -1;
	}
	int address = 0;
	int vmIndex = 0;
	if (instr.args[1][instr.args[1].size() - 1] != ')')
	{
		address = dataLabelHelper(instr.args[1]);
		if (address == -1)
		{
			ASMData reg2 = findReg(instr.args[1]);
			vmIndex = regValues[reg2.getName()];
			//regValues[reg1.getName()] = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	else
	{
		int begin = instr.args[1].find("(") + 1;
		int end = instr.args[1].size() - 1;
		string memref = instr.args[1].substr(begin, end - begin);
		address = dataLabelHelper(memref);
		if (address == -1)
		{
			ASMData reg2 = findReg(memref);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	regValues[reg1.getName()] = (virtualMem[vmIndex + offset] & 0xff);
	return -1;
}

int VMModel::li(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	int imm = atoi(instr.args[1].c_str());
	if (imm == 0 && instr.args[1][0] != '0')
		imm = constants[instr.args[1]];
	regValues[reg1.getName()] = imm;
	return -1;
}

int VMModel::la(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	int address = 0;
	address = dataLabelHelper(instr.args[1]);
	regValues[reg1.getName()] = dataLabels[address].getAddress();
	return -1;
}

int VMModel::sw(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	uint32_t offset = atoi(instr.args[1].c_str());
	if ((offset || instr.args[1][0] == '0') && instr.args[1].find("(") >= instr.args[1].size())
	{
		virtualMem[offset] = regValues[reg1.getName()] & 0xFF;
		virtualMem[offset+1] = regValues[reg1.getName()] & 0xFF00;
		virtualMem[offset+2] = regValues[reg1.getName()] & 0xFF0000;
		virtualMem[offset+3] = regValues[reg1.getName()] & 0xFF000000;
		return -1;
	}
	int address = 0;
	int vmIndex = 0;
	if (instr.args[1][instr.args[1].size() - 1] != ')')
	{
		address = dataLabelHelper(instr.args[1]);
		if (address == -1)
		{
			ASMData reg2 = findReg(instr.args[1]);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	else
	{
		int begin = instr.args[1].find("(") + 1;
		int end = instr.args[1].size()-1;
		string memref = instr.args[1].substr(begin, end - begin);
		address = dataLabelHelper(memref);
		if (address == -1)
		{
			ASMData reg2 = findReg(memref);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	virtualMem[vmIndex + offset] = regValues[reg1.getName()] & 0xFF;
	virtualMem[vmIndex + offset + 1] = (regValues[reg1.getName()] >> 8) & 0xFF;
	virtualMem[vmIndex + offset + 2] = (regValues[reg1.getName()] >> 16) & 0xFF;
	virtualMem[vmIndex + offset + 3] = (regValues[reg1.getName()] >> 24) & 0xFF;
	return -1;
}

int VMModel::sh(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	uint32_t offset = atoi(instr.args[1].c_str());
	if ((offset || instr.args[1][0] == '0') && instr.args[1].find("(") >= instr.args[1].size())
	{
		virtualMem[offset] = regValues[reg1.getName()] & 0xFF;
		virtualMem[offset + 1] = regValues[reg1.getName()] & 0xFF00;
		virtualMem[offset + 2] = regValues[reg1.getName()] & 0xFF0000;
		virtualMem[offset + 3] = regValues[reg1.getName()] & 0xFF000000;
		return -1;
	}
	int address = 0;
	int vmIndex = 0;
	if (instr.args[1][instr.args[1].size() - 1] != ')')
	{
		address = dataLabelHelper(instr.args[1]);
		if (address == -1)
		{
			ASMData reg2 = findReg(instr.args[1]);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	else
	{
		int begin = instr.args[1].find("(") + 1;
		int end = instr.args[1].size() - 1;
		string memref = instr.args[1].substr(begin, end - begin);
		address = dataLabelHelper(memref);
		if (address == -1)
		{
			ASMData reg2 = findReg(memref);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	virtualMem[vmIndex + offset] = regValues[reg1.getName()] & 0xFF;
	virtualMem[vmIndex + offset + 1] = regValues[reg1.getName()] & 0xFF00;
	return -1;
}

int VMModel::sb(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	uint32_t offset = atoi(instr.args[1].c_str());
	if ((offset || instr.args[1][0] == '0') && instr.args[1].find("(") >= instr.args[1].size())
	{
		virtualMem[offset] = regValues[reg1.getName()] & 0xFF;
		virtualMem[offset + 1] = regValues[reg1.getName()] & 0xFF00;
		virtualMem[offset + 2] = regValues[reg1.getName()] & 0xFF0000;
		virtualMem[offset + 3] = regValues[reg1.getName()] & 0xFF000000;
		return -1;
	}
	int address = 0;
	int vmIndex = 0;
	if (instr.args[1][instr.args[1].size() - 1] != ')')
	{
		address = dataLabelHelper(instr.args[1]);
		if (address == -1)
		{
			ASMData reg2 = findReg(instr.args[1]);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	else
	{
		int begin = instr.args[1].find("(") + 1;
		int end = instr.args[1].size() - 1;
		string memref = instr.args[1].substr(begin, end - begin);
		address = dataLabelHelper(memref);
		if (address == -1)
		{
			ASMData reg2 = findReg(memref);
			vmIndex = regValues[reg2.getName()];
		}
		else
			vmIndex = dataLabels[address].getAddress();
	}
	virtualMem[vmIndex + offset] = regValues[reg1.getName()] & 0xFF;
	return -1;
}

int VMModel::move(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	regValues[reg1.getName()] = regValues[reg2.getName()];
	return -1;
}

int VMModel::mfhi(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	regValues[reg1.getName()] = regValues["$hi"];
	return -1;
}

int VMModel::mflo(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	regValues[reg1.getName()] = regValues["$lo"];
	return -1;
}

int VMModel::mthi(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	regValues["$hi"] = regValues[reg1.getName()];
	return -1;
}

int VMModel::mtlo(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	regValues["$lo"] = regValues[reg1.getName()];
	return -1;
}

//Integer Arithmetic
int VMModel::add(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	int op1 = regValues[reg2.getName()];
	int op2 = 0;
	if (reg3.getName() == "")
	{
		op2 = atoi(instr.args[2].c_str());
		if (!op2 && instr.args[2][0] != '0')
			op2 = constants[instr.args[2]];
		regValues[reg1.getName()] = op1 + op2;
	}
	else
	{
		op2 = regValues[reg3.getName()];
		regValues[reg1.getName()] = regValues[reg2.getName()] + op2;
	}
	if (((op1 & 0x80000000) == (op2 & 0x80000000)) && (regValues[reg1.getName()] & 0x80000000) != (op1 & 0x80000000))
	{
		return -2;	//overflow
	}
	return -1;
}

int VMModel::addu(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		if (!imm && instr.args[2][0] != '0')
			regValues[reg1.getName()] = regValues[reg2.getName()] + constants[instr.args[2]];
		else
			regValues[reg1.getName()] = regValues[reg2.getName()] + imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] + regValues[reg3.getName()];
	return -1;
}

int VMModel::sub(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	int op1 = regValues[reg2.getName()];
	int op2 = 0;
	if (reg3.getName() == "")
	{
		op2 = atoi(instr.args[2].c_str());
		if (!op2 && instr.args[2][0] != '0')
			op2 = constants[instr.args[2]];
		regValues[reg1.getName()] = op1 - op2;
	}
	else
	{
		op2 = regValues[reg3.getName()];
		regValues[reg1.getName()] = regValues[reg2.getName()] - op2;
	}
	if (((op1 & 0x80000000) == (op2 & 0x80000000)) && (regValues[reg1.getName()] & 0x80000000) != (op1 & 0x80000000))
	{
		return -2;	//overflow
	}
	return -1;
}

int VMModel::subu(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		if (!imm && instr.args[2][0] != '0')
			regValues[reg1.getName()] = regValues[reg2.getName()] - constants[instr.args[2]];
		else
			regValues[reg1.getName()] = regValues[reg2.getName()] - imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] - regValues[reg3.getName()];
	return -1;
}

int VMModel::mul(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		regValues[reg1.getName()] = regValues[reg2.getName()] * imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] * regValues[reg3.getName()];
	return -1;
}

int VMModel::mulo(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		regValues[reg1.getName()] = regValues[reg2.getName()] * imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] * regValues[reg3.getName()];
	return -1;
}

int VMModel::mulou(Instruction instr)
{
	//ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		regValues["$lo"] = regValues[reg2.getName()] * imm;
	}
	else
		regValues["$lo"] = regValues[reg2.getName()] * regValues[reg3.getName()];
	return -1;
}

int VMModel::mult(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int64_t val1 = (int)regValues[reg1.getName()];
	int64_t val2 = (int)regValues[reg2.getName()];
	val1 = val1 * val2;
	regValues["$hi"] = val1 >> 32;
	regValues["$lo"] = val1 & 0xFFFFFFFF;
	return -1;
}

int VMModel::multu(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int64_t val1 = regValues[reg1.getName()];
	int64_t val2 = regValues[reg2.getName()];
	val1 = val1 * val2;
	regValues["$hi"] = val1 >> 32;
	regValues["$lo"] = val1 & 0xFFFFFFFF;
	return -1;
}

int VMModel::div(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	if (instr.args[2] == "")
	{
		if (regValues[reg2.getName()] != 0)
		{
			regValues["$lo"] = (int)regValues[reg1.getName()] / (int)regValues[reg2.getName()];
			regValues["$hi"] = (int)regValues[reg1.getName()] % (int)regValues[reg2.getName()];
		}
	}
	else
	{
		ASMData reg3 = findReg(instr.args[2]);
		if (regValues[reg3.getName()] != 0)
		{
			regValues[reg1.getName()] = regValues[reg2.getName()] / regValues[reg3.getName()];
		}
	}
	return -1;
}

int VMModel::divu(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	if (instr.args[2] == "")
	{
		if (regValues[reg2.getName()] != 0)
		{
			regValues["$lo"] = regValues[reg1.getName()] / regValues[reg2.getName()];
			regValues["$hi"] = regValues[reg1.getName()] % regValues[reg2.getName()];
		}
	}
	else
	{
		ASMData reg3 = findReg(instr.args[2]);
		if (regValues[reg3.getName()] != 0)
		{
			regValues[reg1.getName()] = regValues[reg2.getName()] / regValues[reg3.getName()];
		}
	}
	return -1;
}

int VMModel::rem(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	if (instr.args[2] == "")
	{
		int imm = atoi(instr.args[2].c_str());
		if (imm != 0)
		{
			regValues[reg1.getName()] = regValues[reg2.getName()] % imm;
		}
	}
	else
	{
		ASMData reg3 = findReg(instr.args[2]);
		if (regValues[reg3.getName()] != 0)
		{
			regValues[reg1.getName()] = regValues[reg2.getName()] % regValues[reg3.getName()];
		}
	}
	return -1;
}

int VMModel::remu(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	if (instr.args[2] == "")
	{
		int imm = atoi(instr.args[2].c_str());
		if (imm != 0)
		{
			regValues[reg1.getName()] = regValues[reg2.getName()] % imm;
		}
	}
	else
	{
		ASMData reg3 = findReg(instr.args[2]);
		if (regValues[reg3.getName()] != 0)
		{
			regValues[reg1.getName()] = regValues[reg2.getName()] % regValues[reg3.getName()];
		}
	}
	return -1;
}

int VMModel::abs(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int num = (int)regValues[reg2.getName()];
	regValues[reg1.getName()] = num < 0 ? -num : num;
	return -1;
}

int VMModel::neg(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	regValues[reg1.getName()] = -regValues[reg2.getName()];
	return -1;
}

int VMModel::negu(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	regValues[reg1.getName()] = -regValues[reg2.getName()];
	return -1;
}

//Logical
int VMModel::andInstr(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		regValues[reg1.getName()] = regValues[reg2.getName()] & imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] & regValues[reg3.getName()];
	return -1;
}

int VMModel::norInstr(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	int op1 = regValues[reg2.getName()];
	int op2 = 0;
	if (reg3.getName() == "")
	{
		op2 = atoi(instr.args[2].c_str());
		if (!op2 && instr.args[2][0] != '0')
			op2 = constants[instr.args[2]];
	}
	else
		op2 = regValues[reg3.getName()];
	regValues[reg1.getName()] = ~(op1 | op2);
	return -1;
}

int VMModel::notInstr(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	if (reg2.getName() == "")
	{
		unsigned int imm = atoi(instr.args[1].c_str());
		if (!imm && instr.args[1][0] != '0')
			imm = constants[instr.args[1]];
		regValues[reg1.getName()] = ~imm;
	}
	else
		regValues[reg1.getName()] = ~regValues[reg2.getName()];
	return -1;
}

int VMModel::orInstr(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		unsigned int imm = atoi(instr.args[2].c_str());
		if (!imm && instr.args[2][0] != '0')
			imm = constants[instr.args[2]];
		regValues[reg1.getName()] = regValues[reg2.getName()] | imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] | regValues[reg3.getName()];
	return -1;
}

int VMModel::xorInstr(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	ASMData reg3 = findReg(instr.args[2]);
	if (reg3.getName() == "")
	{
		int imm = atoi(instr.args[2].c_str());
		if (!imm && instr.args[2][0] != '0')
			imm = constants[instr.args[2]];
		regValues[reg1.getName()] = regValues[reg2.getName()] ^ imm;
	}
	else
		regValues[reg1.getName()] = regValues[reg2.getName()] ^ regValues[reg3.getName()];
	return -1;
}

//Control
int VMModel::j(Instruction instr)
{
	int tlIndex = textLabelHelper(instr.args[0]);
	return textLabels[tlIndex].getPcIndex();
}

int VMModel::beq(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int tlIndex = textLabelHelper(instr.args[2]);
	if (reg2.getName() == "")
	{
		int imm = atoi(instr.args[1].c_str());
		if (regValues[reg1.getName()] == (uint32_t)imm)
			return textLabels[tlIndex].getPcIndex();
	}
	else if(regValues[reg1.getName()] == regValues[reg2.getName()])
		return textLabels[tlIndex].getPcIndex();
	return -1;
}

int VMModel::bne(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int tlIndex = textLabelHelper(instr.args[2]);
	if (reg2.getName() == "")
	{
		int imm = atoi(instr.args[1].c_str());
		if (regValues[reg1.getName()] != (uint32_t)imm)
			return textLabels[tlIndex].getPcIndex();
	}
	else if (regValues[reg1.getName()] != regValues[reg2.getName()])
		return textLabels[tlIndex].getPcIndex();
	return -1;
}

int VMModel::blt(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int tlIndex = textLabelHelper(instr.args[2]);
	if (reg2.getName() == "")
	{
		int imm = atoi(instr.args[1].c_str());
		if (regValues[reg1.getName()] < (uint32_t)imm)
			return textLabels[tlIndex].getPcIndex();
	}
	else if (regValues[reg1.getName()] < regValues[reg2.getName()])
		return textLabels[tlIndex].getPcIndex();
	return -1;
}

int VMModel::ble(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int tlIndex = textLabelHelper(instr.args[2]);
	if (reg2.getName() == "")
	{
		int imm = atoi(instr.args[1].c_str());
		if (regValues[reg1.getName()] <= (uint32_t)imm)
			return textLabels[tlIndex].getPcIndex();
	}
	else if (regValues[reg1.getName()] <= regValues[reg2.getName()])
		return textLabels[tlIndex].getPcIndex();
	return -1;
}

int VMModel::bgt(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int tlIndex = textLabelHelper(instr.args[2]);
	if (reg2.getName() == "")
	{
		int imm = atoi(instr.args[1].c_str());
		if (regValues[reg1.getName()] > (uint32_t)imm)
			return textLabels[tlIndex].getPcIndex();
	}
	else if (regValues[reg1.getName()] > regValues[reg2.getName()])
		return textLabels[tlIndex].getPcIndex();
	return -1;
}

int VMModel::bge(Instruction instr)
{
	ASMData reg1 = findReg(instr.args[0]);
	ASMData reg2 = findReg(instr.args[1]);
	int tlIndex = textLabelHelper(instr.args[2]);
	if (reg2.getName() == "")
	{
		int imm = atoi(instr.args[1].c_str());
		if (regValues[reg1.getName()] >= (uint32_t)imm)
			return textLabels[tlIndex].getPcIndex();
	}
	else if (regValues[reg1.getName()] >= regValues[reg2.getName()])
		return textLabels[tlIndex].getPcIndex();
	return -1;
}