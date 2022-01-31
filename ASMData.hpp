#ifndef ASMDATA_HPP
#define ASMDATA_HPP
#include <iostream>
#include <unordered_map>
#include <inttypes.h>
#include "VMModel.hpp"
//#include "ExecFunctions.hpp"
//#include "parser.hpp"
//#include "VMController.hpp"
using namespace std;

//class Instruction;

class ASMData {
	string name;
	string number;

public:
	ASMData(string name, string number)
	{
		this->name = name;
		this->number = number;
	}

	ASMData()
	{
		name = "";
		number = "";
	}

	//getters
	string getName() { return name; }
	string getNumber() { return number; }
};

class OPCode {
	string name;
	string format;

public:
	//int(*execute)(Instruction instr, VMController &controller);

	/*OPCode(string name, string format)//, int(*execute)(Instruction inst, VMController &controller))
	{
		this->name = name;
		this->format = format;
		//this->execute = execute;
	}*/

	OPCode(string name, string format)
	{
		this->name = name;
		this->format = format;
	}

	OPCode()
	{
		name = "";
		format = "";
	}

	//getters
	string getName() { return name; }
	string getFormat() { return format; }

	void reset() { name = ""; format = ""; }

};

const ASMData findReg(const string name);
const OPCode findOP(const string name);
void initRegisters(unordered_map<string, uint32_t> &registers);
#endif // !ASMDATA_HPP