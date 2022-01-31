#ifndef VMVIEW_HPP
#define VMVIEW_HPP
#include <string>


class VMView {
	std::string prompt;

public:
	VMView() : prompt("simmips> ")
	{ }

	std::string promptUser();
};

#endif // !VMVIEW_HPP
