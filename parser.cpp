#include <iostream>
#include <string.h>
#include <cstdlib>
#include "token.hpp"
#include "ASMData.hpp"
#include "VMModel.hpp"

#define TEXT 0
#define DATA 1
using namespace std;


	///////////////////////////////////////////////////////////
	//////////  STATIC HELPER METHODS  ////////////////////////
	///////////////////////////////////////////////////////////
	bool VMModel::regHelper(TokenList::iterator &it)
	{
		ASMData reg = findReg(it->value());
		if (reg.getName() == "")
		{
			//register was not found and is not supported
			//Parsing Error
			return false;
		}
		//do something with this register
		return true;
	}

	/**
	* @return true if the label is found, false otherwise
	*/
	int VMModel::dataLabelHelper(string label)
	{		
		int index = 0;
		for (auto labels = dataLabels.begin(); labels != dataLabels.end(); ++labels)
		{
			if (labels->getName() == label)
				return index;
			index++;
		}
		return -1;
	}

	/**
		@return the textLabel if it is found, NULL otherwise
	*/
	int VMModel::textLabelHelper(string label)
	{
		int index = 0;
		for (auto labels = textLabels.begin(); labels != textLabels.end(); ++labels)
		{
			if (labels->getName() == label)
				return index;
			index++;
		}
		return -1;
	}

	//determine if this token is an <immediate>
	bool VMModel::immHelper(TokenList::iterator &it)
	{
		if (it->type() == STRING)
		{
			if (atoi(it->value().c_str()) || it->value() == "0")
				return true;
			if (textLabelHelper(it->value()) != -1)
				return true;
			if (constants.find(it->value()) == constants.end())
				return false;
			return true;	//is a valid constant
		}
		//it can't possibly be an integer or a label
		//print error
		return false;
	}

	bool VMModel::memHelper(TokenList::iterator &it)
	{
		if (it->value().size() > 0)	//if it's a string
		{
			if (it->value()[0] == '$')
			{
				if (regHelper(it))	//if it's a register
					return true;
			}
			else if (dataLabelHelper(it->value()) != -1)	//else if its a label
				return true;
			else if (!atoi(it->value().c_str()) && it->value()[0] != '0')	//else if it's an offset for a register
				return false;	//error, invalid token
			else
				it++;	//go to open paren
		}
		if (it->type() == EOL)
		{
			it--;
			return true;
		}
		if (it->type() != OPEN_PAREN)
			return false;	//error
		it++;	//go to register
		ASMData reg = findReg(it->value());
		if (reg.getName() == "")
			if (dataLabelHelper(it->value()) == -1)	//register not found
				return false;
		it++;	//go to end paren
		if (it->type() != CLOSE_PAREN)
			return false;	//error
	//false if "does not match instruction format"
		return true;
	}

	bool VMModel::srcHelper(TokenList::iterator &it)
	{
		if (it->value()[0] == '$')	//it should be interpreted as a register
			return regHelper(it);
		else
			return immHelper(it);
	}

	bool VMModel::specialHelper(TokenList::iterator &it)
	{
		//These are special because they have different formats
		if (it->value() == "div" || it->value() == "divu")
		{
			it++;	//go to first arg
			if (!regHelper(it))
				return false;	//invalid instruction format
			it++;	//go to separator
			if (it->type() != SEP)
				return false;	//invalid instruction format
			it++;	//go to reg
			if (!regHelper(it))
				return false;	//invalid instruction format
			it++;	//go to next token
			if (it->type() == EOL)
			{
				it--;	//it is incremented later as well to check EOL case
				return true;
			}
			if (it->type() != SEP)
				return false;	//invalid instruction format
			it++;	//go to src
			if (!srcHelper(it))
				return false;	//invalid instruction format
		}
		return true;
	}

	bool VMModel::parseConstant(TokenList::iterator &it)
	{
		if (it->type() != STRING)
			return false;	//invalid token
		if ((it->value()[0] >= 'a' && it->value()[0] <= 'z') || (it->value()[0] >= 'A' && it->value()[0] <= 'Z'))	//is alpha character
			for (size_t i = 1; i < it->value().size() - 1; i++)
			{
				if ((it->value()[i] < 'a' || it->value()[i] > 'z') && (it->value()[i] < 'A' || it->value()[i] > 'Z'))
					return false;	//value on this line contains non alpha-numeric characters
			}
		else
			return false; //value on this line contains non alpha-numeric characters
		string name = it->value();
		it++;	//go to equal sign
		if (it->type() != EQUAL)
			return false;	//incorrect format for constant
		it++;	//go to integer
		int i = atoi(it->value().c_str());
		if (!i && it->value()[0] != '0')
			return false;	//incorrect format for constant
		constants[name] = i;
		return true;
	}

	static int parseIntLayout(string val)
	{
		if (val == ".word")
			return 4;
		else if (val == ".half")
			return 2;
		else if (val == ".byte")
			return 1;
		else if (val == ".space")
			return -1;	//needs to be handled later
		return 0;
	}

	static bool parseStringLayout(string val)
	{
		if (val == ".ascii")
			return true;
		else if (val == ".asciiz")
			return true;
		return false;
	}

	bool VMModel::parseString(TokenList::iterator &it, DataLabel &dl)
	{
		if (it->type() != STRING_DELIM)
			return false;	//invalid string format
		it++;	//go to string
		dl.setAddress(virtualMem.size());
		if (it->type() == STRING)
		{
			size_t i;
			for (i = 0; i < it->value().size(); i++)
			{
				virtualMem.push_back(it->value()[i]);
			}
			dl.setSize(i);
			it++;	//go to end string delim
			if (it->type() != STRING_DELIM)
				return false;	//invalid string format
		}
		else if (it->type() != STRING_DELIM)
			return false;	//invalid string format
		return true;
	}

	bool VMModel::parseLayout(TokenList::iterator &it, int dlIndex)
	{
		int length = parseIntLayout(it->value());
		if (length)
		{
			string type = it->value();
			if(dlIndex >= 0)
				dataLabels[dlIndex].setSize(length);
			it++;
			if (it->type() != STRING)
				return false;
			int value = atoi(it->value().c_str());
			if (!value && it->value()[0] != '0')
				value = constants[it->value()];
			if (dlIndex >= 0)
				dataLabels[dlIndex].setAddress(virtualMem.size());
			int mask = 0xFF;
			if (type == ".space")
			{
				if (dlIndex >= 0)
					dataLabels[dlIndex].setSize(value);
				for (int i = 0; i < value; i++)
					virtualMem.push_back(0);
			}
			else
			{
				for (int i = 0; i < length; i++)
				{
					virtualMem.push_back(value & mask);
					value >>= 8;	//shift down a byte
				}
			}
			it++;
			while (it->type() != EOL)
			{
				if (it->type() != SEP)
					return false;	//expected separator
				it++;	//go to integer
				if (!(value = atoi(it->value().c_str())) && it->value()[0] != '0')
					return false;	//expected integer
				for (int i = 0; i < length; i++)
				{
					virtualMem.push_back(value & mask);
					value >>= 8;	//shift down a byte
				}
				it++;	//go back to separator
			}
			return true;
		}
		else if (parseStringLayout(it->value()))
		{
			it++;	//go to string
			if (dlIndex >= 0)
				parseString(it, dataLabels[dlIndex]);
			else
			{
				DataLabel dl;	//dummy label
				parseString(it, dl);
			}
			it++;	//move to EOL as int_layout does
		}
		return true;	//invalid layout specifier
	}

	bool VMModel::parseDeclaration(TokenList::iterator &it)
	{
		if (it->type() == STRING)
		{
			int dlIndex = -1;
			if (it->value()[it->value().size() - 1] == ':')
			{
				dlIndex = dataLabelHelper(it->value().substr(0, it->value().size() - 1));
				if (dataLabels[dlIndex].getName() != "")
					it++;
				//else error
				else
					return false;
			}
			if (it->type() != EOL)
			{
				if (it->value()[0] == '.')
				{
					return parseLayout(it, dlIndex);
				}
				else if (parseConstant(it))
				{
					it++;	//move from last parsed token to EOL
					return true;
				}
				else
					return false;	//error
			}

		}
		return false;	//invalid token type

	}

	/*static int lineLength(TokenList tokens)
	{
		int lineLength = 0;
		auto it = tokens.begin();
		int lineNumber = it->line();
		while (it->line() == lineNumber)
		{
			lineLength++;
			it++;
		}
	}*/

	void VMModel::resetModel()
	{
		constants.clear();
		virtualMem.clear();
		pc.clear();
		dataLabels.clear();
		textLabels.clear();
	}

	void VMModel::parseLabels(TokenList tokens)
	{
		int mode = DATA;
		bool foundLabel = false;	//know when to skip the rest of the line
		for (auto it = tokens.begin(); it != tokens.end(); it = ++it)
		{
			if (it->value() == ".data")
			{
				mode = DATA;
				continue;
			}
			else if (it->value() == ".text")
			{
				mode = TEXT;
				continue;
			}

			if (!foundLabel && it->type() == STRING)
			{
				if (it->value()[it->value().size() - 1] == ':')	//if it ends in a ':', it is a label
				{
					if (mode == DATA)
					{
						DataLabel label(it->value().substr(0, it->value().size() - 1));
						dataLabels.push_back(label);
					}
					else
					{
						TextLabel label(it->value().substr(0, it->value().size() - 1));
						textLabels.push_back(label);
					}
					foundLabel = true;
				}
				else if (!foundLabel && mode == DATA)
				{
					if (it->type() == STRING)	//it is a constant
					{
						if (it->value()[0] == '.')
						{
							foundLabel = true;	//unnamed label
						}
						else
						{
							//its a constant for sure, so handle it later
							constants[it->value()] = 0;	//initialize it
							foundLabel = true;
						}
					}
				}
			}
			else if (it->type() == EOL)
				foundLabel = false;	//reset after each line
		}
	}

	int VMModel::parse(TokenList tokens)
	{
		resetModel();
		parseLabels(tokens);
		bool mode = DATA;	//if data mode is true, we are in the .data section, .text section otherwise
		OPCode operation;
		for (auto it = tokens.begin(); it != tokens.end(); it = ++it)
		{
			if (it->type() == STRING)
			{
				if (it->value() == ".data")
				{
					mode = DATA;
					it++;
					continue;
				}
				else if (it->value() == ".text")
				{
					mode = TEXT;
					it++;
					continue;
				}
			}

			if (mode == DATA)    //.data
			{
				if (!parseDeclaration(it))
					return 2;	//parsing error
				if (it->type() != EOL)
					return 2;	//too many tokens
			}
			else    //.text
			{
				if (textLabelHelper(it->value().substr(0, it->value().size() - 1)) != -1)
				{
					operation.reset();
					it++;
				}
				if (it->type() == STRING)
				{
					operation = findOP(it->value());	//look up the op code
					if (operation.getName() == "")
						return 2;
					if (operation.getFormat() != "*")
						it++;
				}

				size_t x = 0;	//guard against too many tokens error(and going out of bounds)
				while (it->type() != EOL)
				{
					if ((x >= operation.getFormat().size()) && (operation.getName() != "nop"))
					{
						//an error occurred
						//too many tokens in the instruction
						return 2;
					}

					switch (operation.getFormat()[x])
					{
					case 'r':
						if (!regHelper(it))
							return 2;
						break;
					case 'm':
						if (!memHelper(it))
							return 2;
						break;
					case 's':
						if (!srcHelper(it))
							return 2;
						break;
					case 'i':
						if (!immHelper(it))
							return 2;
						break;
					case ',':
						if (it->type() != SEP)
						{
							//Parsing Error
							return 2;
						}
						break;
					case 'l':
						if (textLabelHelper(it->value()) == -1)
							return 2;
						break;
					case '*':
						if (!specialHelper(it))
							return 2;
						break;
					default:
						cout << "Error in lookup table" << endl;
					}
					x++;
					it++;
				}
				if (x < operation.getFormat().size())
					return 2;	//not enough arguments
			}
		}
		return 0;	//ok
	}

	void VMModel::preProcessor(TokenList tokens)
	{
		bool textSection = false;	//skip data secion
		for (auto it = tokens.begin(); it != tokens.end(); ++it)
		{
			if (it->value() == ".text")
			{
				textSection = true;
				continue;
			}
			if (it->value() == ".data")
			{
				textSection = false;
				continue;
			}

			if (textSection)
			{
				if (it->type() == STRING)
				{
					if (it->value()[it->value().size() - 1] != ':')	//line doesn't start with a label
					{
						Instruction instr(pc.size(), it->value());
						int index = 0;
						while (it->type() != EOL)
						{
							string arg = "";
							while ((++it)->type() != SEP && it->type() != EOL)
							{
								switch (it->type())
								{
								case STRING:
									arg += it->value();
									break;
								case OPEN_PAREN:
									arg += "(";
									break;
								case CLOSE_PAREN:
									arg += ")";
									break;
								case STRING_DELIM:
									arg += "\"";
									break;
								default:
									break;
								}
							}
							instr.args[index] = arg;
							index++;
						}
						pc.push_back(instr); //Add it to the list of instruction
					}
					else
					{
						int tlIndex = textLabelHelper(it->value().substr(0, it->value().size() - 1));
						textLabels[tlIndex].setPcIndex(pc.size());
					}
				}
			}
		}
	}