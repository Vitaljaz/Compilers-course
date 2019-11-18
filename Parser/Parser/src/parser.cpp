#include "parser.h"

Parser::Parser(const std::string & fileName_)
{
	lexer = Lexer(fileName_);
}

void Parser::run()
{
	getLexems();
	token = tokenList.front();
	statements();
}

void Parser::createError(unsigned line, unsigned errorNumber)
{
}

void Parser::getLexems()
{
	lexer.printLexemList();
	tokenList = lexer.getTokenList();
}

void Parser::move()
{
	prevToken = token;
	tokenList.erase(tokenList.begin());
	if (!tokenList.empty())
	{
		token = tokenList.front();
	}
	else
	{
		LOG("End tokens\n")
	}
}

void Parser::statements()
{
	while (!tokenList.empty())
	{
		statement();
	}

	LOG("OK")
}

void Parser::statement()
{
	if (first)
		move();

	first = true;

	if (token.tokenClass == "type")
	{
		LOG("type")
		if (local_var())
		{
			LOG("type ID; - done")
			statement();
		}
	}
	else if (token.lexeme == "if")
	{
		LOG("IF")
		if (br_open())
		{
			LOG("IF (")
			if (expression())
			{
				LOG("CONSTRUCTION if (expression) - done")
				statement();
			}
		}
		else
		{

		}
	}
	else if (token.lexeme == "for")
	{

	}
	else if (token.lexeme == "while")
	{

	}
	else if (token.lexeme == "break")
	{
		if (end_lexeme())
		{
			LOG("break; - good lexeme")
			return;
		}
		else
		{
			createError(token.lineNumber, 0);
		}
	}
	else if (token.lexeme == "continue")
	{
		if (end_lexeme())
		{
			LOG("continue; - good lexeme")
				return;
		}
		else
		{
			createError(token.lineNumber, 0);
			return;
		}
	}
	else if (token.lexeme == ";")
	{
		return;
	}
	else if (token.lexeme == "{")
	{
		LOG("{")
		bracketsList.push(Brackets::O_BR);
		statement();
	}
	else if (token.lexeme == "}")
	{
		if (bracketsList.empty())
		{
			createError(token.lineNumber, 1); // not find pair for bracket
		}
		else
		{
			if (bracketsList.top() == Brackets::O_BR)
			{
				LOG("{ statement } - done")
				bracketsList.pop();
				return;
			}
			else
			{
				bracketsList.push(Brackets::C_BR);
			}
		}
	}
	else if (token.tokenClass == "operand")
	{
		// if prev token ; 
	}
}

bool Parser::expression()
{
	move();

	if (token.tokenClass == "unary operator")
	{
		if (prevToken.tokenClass != "unary")
		{
			if (expression())
			{
				return true;
			}
			else
			{
				return false; // error
			}
		}
		else
		{
			return false; // error
		}
	}
	else if (token.tokenClass == "identifier") // need operand
	{
		LOG("ID")
		if (what_expression())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool Parser::what_expression()
{
	move();
	
	if (token.tokenClass == "logical operator")
	{
		LOG("&&")
		if (expression())
		{
			LOG("EXP")
			return true;
		}
		else 
		{
			return false;
		}
	}
	else if (token.tokenClass == "relational operator")
	{
		if (expression())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (token.tokenClass == "arithmetic operator")
	{
		if (expression())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (token.lexeme == ")")
	{
		return true;
	}

	return false;
}

bool Parser::local_var()
{
	if (local_var_decl())
	{
		LOG("type identifier")
		if (local_var_list())
		{
			LOG("type identifier ,")
			if (local_var())
			{
				return true;
			}
		}
		else
		{
			if (local_var_init())
			{
				LOG("type identifier = ")
				if (local_var_init())
				{
					LOG("type identifier = op")
					return true;
				}
			}
			else if (local_var_end())
			{
				LOG("type identifier;")
				return true;
			}
		}
	}

	return false;
}

bool Parser::local_var_end()
{
	move();

	if (token.lexeme == ";")
	{
		return true;
	}

	return false;
}

bool Parser::local_var_r()
{
	move();
	if (token.tokenClass == "identifier" || token.tokenClass == "digit") // operand
	{
		if (local_var_end())
		{
			return true;
		}
	}
	
	return false;
}

bool Parser::local_var_init()
{
	move();
	
	if (token.tokenClass == "=")
	{
		return true;
	}
	return false;
}
bool Parser::local_var_decl()
{
	move();

	if (token.tokenClass == "identifier")
	{
		return true;
	}
	return false;
}

bool Parser::local_var_list()
{
	move();

	if (token.lexeme == ",")
	{
		return true;
	}
	return false;
}

bool Parser::end_lexeme()
{
	move();

	if (token.lexeme == ";")
		return true;

	return false;
}

bool Parser::br_open()
{
	move();

	if (token.lexeme == "(")
		return true;

	return false;
}

bool Parser::br_close()
{
	move();

	if (token.lexeme == ")")
		return true;

	return false;
}
