#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <inttypes.h>
#include "VMController.hpp"
#include "ASMData.hpp"
using namespace std;

VMController::VMController(VMModel vm, VMView gui)
{
	this->vm = vm;
	this->gui = gui;
	pcIndex = 0;
	statusString = "";

	REPL();
}

VMController::VMController(VMModel vm)
{
	this->vm = vm;
	pcIndex = 0;
	statusString = "";
}

VMController::~VMController() { }

void VMController::runThread()
{
	while (run)
	{
		stepOnce();
	}
}

void VMController::REPL()
{
	while (parseInput(gui.promptUser()) != "quit") { }
}

bool VMController::stepOnce()
{
	if ((uint16_t)(pcIndex) >= vm.pc.size())
		return false;
	Instruction current = vm.pc.at(pcIndex);
	int jump = vm.execWrapper(current);
	if (jump != -1)
		pcIndex = jump;
	else
	{
		pcIndex++;
	}
	return true;
}

string VMController::parseInput(string in)
{
	istringstream iss(in);
	stringstream response;
	string command;
	string arg;
	iss >> command;
	iss >> arg;
	if (command == "step" && !run)
	{
		if (!stepOnce())
			return "quit";
		response << "0x" << std::setfill('0') << std::setw(8) << std::hex << pcIndex;
		cout << response.str() << endl;
		return response.str();
	}
	else if (command == "print" && !run)
	{
		if (arg[0] == '$')
		{
			if (arg == "$pc")
			{
				response << "0x" << std::setfill('0') << std::setw(8) << std::hex << pcIndex;
				cout << response.str() << endl;
				return response.str();
			}
			ASMData reg = findReg(arg);
			if (reg.getName() == "")
			{
				response << "Error: register not found.";
				cerr << response.str() << endl;
				return response.str();
			}
			response << "0x" << std::setfill('0') << std::setw(8) << std::hex << vm.regValues[reg.getName()];
			cout << response.str() << endl;
		}
		else
		{
			//address
			uint16_t memLocation = stoi(arg.substr(3), nullptr, 16);
			if (memLocation >= vm.virtualMem.size())
			{
				response << "0x00000000";
				cout << response.str() << endl;
			}
			else
			{
				response << "0x" << std::setfill('0') << std::setw(2) << std::hex << ((uint16_t)(vm.virtualMem[memLocation]) & 0xff);
				cout << response.str() << endl;
			}
		}
		return response.str();
	}
	else if (command == "status")
	{
		response << statusString;
		cout << response.str();
		return response.str();
	}
	else if (command == "quit")
	{
		run = false;
		if (th1.joinable())
			th1.join();
		return "quit";
	}
	else if (command == "run" && !run)
	{
		run = true;
		th1 = std::thread(&VMController::runThread, this);
		statusString = "Running";
		return "running";
	}
	else if (command == "break")
	{
		run = false;
		if (th1.joinable())
			th1.join();
		if ((uint16_t)(pcIndex) >= vm.pc.size())
			statusString = "Program is finished running.";
		return "success";
	}
	else if(!run)
	{
		response << "Error: unknown command";
		cerr << response.str() << endl;
		return response.str();
	}
	else
	{
		response << "Program is running, stop it with 'break'";
		cerr << response.str() << endl;
		return response.str();
	}
}

