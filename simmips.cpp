// implement entry point for simmips here
#include <iostream>
#include <fstream>
#include <QApplication>
#include "lexer.hpp"
#include "VMModel.hpp"
#include "VMController.hpp"
#include "VMView.hpp"
#include "virtual_machine_gui.hpp"

int main(int argc, char *argv[])
{
	bool gui = false;
	int fileArg;
	if (argc < 2)
	{
		std::cout << "Error: No input file provided" << std::endl;
		return EXIT_FAILURE;
	}
	if (argc > 2)
	{
		if (!strcmp(argv[1], "--gui"))
		{
			gui = true;
			fileArg = 2;
		}
		else if (!strcmp(argv[2], "--gui"))
		{
			gui = true;
			fileArg = 1;
		}
	}
	else
		fileArg = 1;
	//std::ifstream in(argv[fileArg]);
	
	if (gui)	//gui
	{
		//load gui mode
		QApplication app(argc, argv);
		VirtualMachineGUI GUI;
		GUI.load(argv[fileArg]);
		GUI.showMaximized();
		return app.exec();
	}
	else
	{
		VMView gui;
		VMModel vm(argv[fileArg]);
		if (vm.status)
			return EXIT_FAILURE;
		VMController controller(vm, gui);
	}
	return EXIT_SUCCESS;
}
