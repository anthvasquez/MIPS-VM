#include <iostream>
#include <sstream>
#include "VMView.hpp"
#include "VMController.hpp"
using namespace std;

string VMView::promptUser()
{
	string response;
	cout << prompt;
	getline(cin, response);
	return response;
}