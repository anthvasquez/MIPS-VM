#include <iostream>
#include <fstream>
#include "ASMData.hpp"
#include "VMModel.hpp"
#include "lexer.hpp"
//#include "ASMData.hpp"
using namespace std;



VMModel::VMModel() :
	virtualMem(),
	constants(),
	textLabels(),
	dataLabels(),
	pc(),
	regValues()	//34 registers
{
	initRegisters(regValues);
	//initExec();
}

VMModel::VMModel(string filename) :
	virtualMem(),
	constants(),
	textLabels(),
	dataLabels(),
	pc(),
	regValues()
{
	initRegisters(regValues);
	//initExec();
	std::ifstream in(filename);
	if (!in)
	{
		cerr << "Error: File could not be opened." << endl;
		status = 1;	//file error status
		return;
	}
	TokenList tl = tokenize(in);
	in.close();
	status = parse(tl);
	if (!status)	//0 means ok
		preProcessor(tl);
}

VMModel::VMModel(istream &in) :
	virtualMem(),
	constants(),
	textLabels(),
	dataLabels(),
	pc(),
	regValues()
{
	initRegisters(regValues);
	//initExec();
	TokenList tl = tokenize(in);
	status = parse(tl);
	if(!status)
		preProcessor(tl);
}

VMModel::~VMModel()
{
	virtualMem.clear();
	constants.clear();
	textLabels.clear();
	dataLabels.clear();
	pc.clear();
	regValues.clear();
	//exec.clear();
}

/**
* unordered_map<string, execFunction> would not work, so this is plan B
*/
int VMModel::execWrapper(Instruction instr)
{
	if (instr.getName() == "lh")
		return lh(instr);
	else if (instr.getName() == "lb")
		return lb(instr);
	else if (instr.getName() == "li")
		return li(instr);
	else if (instr.getName() == "la")
		return la(instr);
	else if (instr.getName() == "lw")
		return lw(instr);
	else if (instr.getName() == "sw")
		return sw(instr);
	else if (instr.getName() == "sh")
		return sh(instr);
	else if (instr.getName() == "sb")
		return sb(instr);
	else if (instr.getName() == "move")
		return move(instr);
	else if (instr.getName() == "mfhi")
		return mfhi(instr);
	else if (instr.getName() == "mthi")
		return mthi(instr);
	else if (instr.getName() == "mflo")
		return mflo(instr);
	else if (instr.getName() == "mtlo")
		return mtlo(instr);
	else if (instr.getName() == "add")
		return add(instr);
	else if (instr.getName() == "addu")
		return addu(instr);
	else if (instr.getName() == "sub")
		return sub(instr);
	else if (instr.getName() == "subu")
		return subu(instr);
	else if (instr.getName() == "mul")
		return mul(instr);
	else if (instr.getName() == "mulo")
		return mulo(instr);
	else if (instr.getName() == "mulou")
		return mulou(instr);
	else if (instr.getName() == "mult")
		return mult(instr);
	else if (instr.getName() == "multu")
		return multu(instr);
	else if (instr.getName() == "div")
		return div(instr);
	else if (instr.getName() == "divu")
		return divu(instr);
	else if (instr.getName() == "rem")
		return rem(instr);
	else if (instr.getName() == "remu")
		return remu(instr);
	else if (instr.getName() == "abs")
		return abs(instr);
	else if (instr.getName() == "neg")
		return neg(instr);
	else if (instr.getName() == "negu")
		return negu(instr);
	else if (instr.getName() == "and")
		return andInstr(instr);
	else if (instr.getName() == "nor")
		return norInstr(instr);
	else if (instr.getName() == "not")
		return notInstr(instr);
	else if (instr.getName() == "or")
		return orInstr(instr);
	else if (instr.getName() == "xor")
		return xorInstr(instr);
	else if (instr.getName() == "j")
		return j(instr);
	else if (instr.getName() == "beq")
		return beq(instr);
	else if (instr.getName() == "bne")
		return bne(instr);
	else if (instr.getName() == "blt")
		return blt(instr);
	else if (instr.getName() == "ble")
		return ble(instr);
	else if (instr.getName() == "bgt")
		return bgt(instr);
	else if (instr.getName() == "bge")
		return bge(instr);
	else
		return -1;	//nop
}