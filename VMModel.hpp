#ifndef  VMMODEL_HPP
#define VMMODEL_HPP
#include "token.hpp"
#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include <unordered_map>
#include <list>
#include <fstream>
#include <vector>
#include <functional>
using namespace std;

#define NUMREGISTERS 34


class Instruction {
	std::string name;
	int lineNumber;
public:
	std::string args[3];

	Instruction(int line, std::string name)
	{
		this->name = name;
		lineNumber = line;
		args[0] = "";
		args[1] = "";
		args[2] = "";
	}

	Instruction()
	{
		lineNumber = -1;
	}

	int getLine() { return lineNumber; }
	std::string getName() { return name; }
};

class DataLabel {
	string name;
	int size;
	int startAddress;	//start address in the address space
public:
	DataLabel(string n, int addy)
	{
		name = n;
		size = 0;
		startAddress = addy;
	}

	DataLabel(string n) { name = n; size = 0; startAddress = -1; }
	DataLabel() { name = "";  size = 0; startAddress = -1; }

	string getName() { return name; }
	int getSize() { return size; }
	int getAddress() { return startAddress; }

	void setSize(int s) { size = s; }
	void setAddress(int a) { startAddress = a; }
};

class TextLabel {
	string name;
	int pcIndex;	//the instr in the pc this label points to
public:
	TextLabel(string n) { name = n; pcIndex = 0; }
	TextLabel() { name = ""; pcIndex = 0; }

	string getName() { return name; }
	int getPcIndex() { return pcIndex; }
	void setPcIndex(int i) { pcIndex = i; }
};

/*class RegisterValue {
public:
	uint32_t vmIndex;	//ref to space in virtual memory
	uint32_t value;	//constant value
	bool vmTarget;	//use vmIndex if true, value otherwise

	RegisterValue();
};*/

class VMModel {
public:
	vector<int8_t> virtualMem;	//virtual memory space that contains all .data (address space)
	unordered_map<string, int> constants;
	vector<TextLabel> textLabels;
	vector<DataLabel> dataLabels;
	vector<Instruction> pc;	//contains all the instructions
	unordered_map<string, uint32_t> regValues;
	//unordered_map<string, int(VMModel::*)(Instruction)> exec;
	int status;
	

	VMModel();
	VMModel(istream &in);
	VMModel(string filename);
	~VMModel();

	int execWrapper(Instruction instr);

	//parsing
	bool regHelper(TokenList::iterator &it);
	int dataLabelHelper(string label);
	int textLabelHelper(string label);
	bool immHelper(TokenList::iterator &it);
	bool memHelper(TokenList::iterator &it);
	bool srcHelper(TokenList::iterator &it);
	bool specialHelper(TokenList::iterator &it);
	int parseInteger(string val);
	bool parseConstant(TokenList::iterator &it);
	bool parseString(TokenList::iterator &it, DataLabel &dl);
	bool parseLayout(TokenList::iterator &it, int dlIndex);
	bool parseDeclaration(TokenList::iterator &it);
	void resetModel();

	void preProcessor(TokenList tokens);
	void parseLabels(TokenList tokens);
	int parse(TokenList tokens);

	//int nop(Instruction instr);
	//Data Movement
	int lw(Instruction instr);
	int lh(Instruction instr);
	int lb(Instruction instr);
	int li(Instruction instr);
	int la(Instruction instr);
	int sw(Instruction instr);
	int sh(Instruction instr);
	int sb(Instruction instr);
	int move(Instruction instr);
	int mfhi(Instruction instr);
	int mflo(Instruction instr);
	int mthi(Instruction instr);
	int mtlo(Instruction instr);
	//Integer Arithmetic
	int add(Instruction instr);
	int addu(Instruction instr);
	int sub(Instruction instr);
	int subu(Instruction instr);
	int mul(Instruction instr);
	int mulo(Instruction instr);
	int mulou(Instruction instr);
	int mult(Instruction instr);
	int multu(Instruction instr);
	int div(Instruction instr);
	int divu(Instruction instr);
	int rem(Instruction instr);
	int remu(Instruction instr);
	int abs(Instruction instr);
	int neg(Instruction instr);
	int negu(Instruction instr);
	//Logical
	int andInstr(Instruction instr);
	int norInstr(Instruction instr);
	int notInstr(Instruction instr);
	int orInstr(Instruction instr);
	int xorInstr(Instruction instr);
	//Control
	int j(Instruction instr);
	int beq(Instruction instr);
	int bne(Instruction instr);
	int blt(Instruction instr);
	int ble(Instruction instr);
	int bgt(Instruction instr);
	int bge(Instruction instr);

	//void initExec();
};
#endif // ! VMMODEL_HPP
