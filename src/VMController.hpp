#ifndef VMCONTROLLER_HPP
#define VMCONTROLLER_HPP

#include <thread>
#include "VMModel.hpp"
#include "VMView.hpp"

class VMController {
	std::thread th1;
	bool run;
public:
	VMModel vm;
	VMView gui;
	int pcIndex;	//next instruction to execute from $pc
	std::string statusString;

	VMController(VMModel vm, VMView gui);
	VMController(VMModel vm);
	~VMController();

	void runThread();
	void REPL();
	bool stepOnce();
	string parseInput(std::string in);

	void setGUI(VMView gui) { this->gui = gui; }
};

#endif // !VMCONTROLLER_HPP
