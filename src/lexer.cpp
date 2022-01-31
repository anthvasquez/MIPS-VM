#include <iostream>
#include <string.h>
#include <stack>
#include "lexer.hpp"
#include "token.hpp"
using namespace std;

bool meaningfulLine = false;	//checks for non-empty lines which require an EOL

/**
 * Wrapper function to create the string token
 *
 * @param tokens The list of tokens to append to
 * @param buf The string buffer containing the token string
 * @param index The index at which the token appears in the input stream
 *
 * In hindsight, I should have matched the order of the parameters
 * that is used for push_back...
 */
static void flushBuffer(TokenList &tokens, string &buf, int index)
{
	if (buf.size() != 0) {	//flush buffer
		tokens.push_back((Token(TokenType::STRING, index, buf)));
		meaningfulLine = true;
		buf = "";
	}
}

// implement the lexer function here

/**
* Creates a list of tokens from the input stream
*
* @return A TokenList of the converted input
*/
TokenList tokenize(std::istream & ins)
{
	TokenList tokens;
	string line;	//read in the current line
	string buf;		//store the token substring of line
	int index = 1;	//count the line number 
	bool quoteMode = false;	//inside of quotation marks
	meaningfulLine = false;
	stack<int> parenCheck;	//keep track of open and closing parenthesis, value is unimportant

	while (getline(ins, line, '\n'))
	{
		for(size_t i = 0; i < line.size(); i++)
		{
			if (line[i] == '#')	{ //make sure to skip comments
				flushBuffer(tokens, buf, index);
				break;
			}

			if (!quoteMode) {	//not inside of quotation marks
				switch (line[i])
				{
				case ',':
					flushBuffer(tokens, buf, index);
					tokens.push_back((Token(TokenType::SEP, index)));
					meaningfulLine = true;
					break;
				case ' ':
					flushBuffer(tokens, buf, index);
					break;
				case '\t':
					flushBuffer(tokens, buf, index);
					break;
				case '(':
					flushBuffer(tokens, buf, index);
					parenCheck.push(index);
					tokens.push_back(Token(TokenType::OPEN_PAREN, index));
					meaningfulLine = true;
					break;
				case ')':
					flushBuffer(tokens, buf, index);
					if (parenCheck.size() == 0)	//mismatched parentheses
					{
						tokens.push_back(Token(TokenType::ERROR, index, "Error: unmatched paren on line " + to_string(index) + ".\n"));
						return tokens;
					}
					parenCheck.pop();
					tokens.push_back(Token(TokenType::CLOSE_PAREN, index));
					meaningfulLine = true;
					break;
				case '"':
					quoteMode = true;
					flushBuffer(tokens, buf, index);
					tokens.push_back(Token(TokenType::STRING_DELIM, index));
					meaningfulLine = true;
					break;
				case '=':
					flushBuffer(tokens, buf, index);
					tokens.push_back(Token(TokenType::EQUAL, index));
					meaningfulLine = true;
					break;
				default:
					if (line[i] >= '!')	//make sure it is a character
						buf += line[i];
				}
			}
			else
			{
				if (line[i] != '"')
					buf += line[i];
				else
				{
					//empty strings are ok here
					tokens.push_back(Token(TokenType::STRING, index, buf));
					buf = "";
					tokens.push_back(Token(TokenType::STRING_DELIM, index));
					meaningfulLine = true;
					quoteMode = false;	//end quote mode
				}
			}
		}

		flushBuffer(tokens, buf, index);
		
		if (quoteMode) {//no end quote ended quotation mode
			tokens.push_back(Token(TokenType::ERROR, index, "Error: unmatched quote on line " + to_string(index) + ".\n"));
			return tokens;
		}

		if (parenCheck.size() != 0) {//make sure all parentheses are aligned
			tokens.push_back(Token(TokenType::ERROR, index, "Error: unmatched paren on line " + to_string(parenCheck.top()) + ".\n"));
			return tokens;
		}

		if (meaningfulLine) {//add an EOL unless the input produces no other tokens
			tokens.push_back((Token(TokenType::EOL, index)));
			meaningfulLine = false;
		}

		index++;
	}
	return tokens;
}
