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

void Parser::createError(unsigned line, ErrorType errorNumber)
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
		if (local_var())
		{
			statement();
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_ID);
		}
	}
	else if (token.lexeme == "if")
	{
		if (br_open())
		{
			if (expression())
			{
				LOG("CONSTRUCTION if (expression) - done")
					statement();
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_BR_O);
		}
	}
	else if (token.lexeme == "for")
	{

	}
	else if (token.lexeme == "while")
	{
		if (br_open())
		{
			if (expression())
			{
				LOG("CONSTRUCTION while (expression) - done")
					statement();
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_BR_O);
		}
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
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
			return;
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
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
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
			createError(token.lineNumber, ErrorType::MISS_F_BR_O); // not find pair for bracket
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
		if (prevToken.lexeme == ";")
		{
			if (statement_exp_start())
			{
				statement_exp();
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
		}
	}
}


bool Parser::expression()
{
	move();

	if (token.tokenClass == "unary operator")
	{
		if (prevToken.tokenClass != "unary operator")
		{
			if (expression())
			{
				return true;
			}
			else
			{
				createError(token.lineNumber, ErrorType::EXP_START);
				return false;
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::DOUBLE_UNARY);
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
			createError(token.lineNumber, ErrorType::EXP_MISS_OP);
			return false;
		}
	}

	return false;
}

bool Parser::statement_exp()
{
	move();

	if (token.tokenClass == "identifier") // operand
	{
		if (what_statement_exp())
		{
			return true;
		}
		else
		{
			createError(token.lineNumber, ErrorType::ST_EXP_MISS_OP);
			return false;
		}
	}
	else if (token.tokenClass == "unary operator")
	{
		if (prevToken.tokenClass != "unary operator")
		{
			if (statement_exp())
			{
				return true;
			}
			else
			{
				createError(token.lineNumber, ErrorType::MISS_ID);
				return false;
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::DOUBLE_UNARY);
			return false;
		}
	}
}

bool Parser::what_statement_exp()
{
	move();

	if (token.tokenClass == "logical operator")
	{
		LOG("&&")
			if (statement_exp())
			{
				LOG("EXP")
					return true;
			}
			else
			{
				createError(token.lineNumber, ErrorType::MISS_OP);
				return false;
			}
	}
	else if (token.tokenClass == "relational operator")
	{
		if (statement_exp())
		{
			return true;
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_OP);
			return false;
		}
	}
	else if (token.tokenClass == "arithmetic operator")
	{
		if (statement_exp())
		{
			return true;
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_OP);
			return false;
		}
	}
	else if (token.lexeme == ";")
	{
		return true;
	}

	return false;
}

bool Parser::statement_exp_start()
{
	move();
	
	if (token.lexeme == "=")
	{
		return true;
	}

	createError(token.lineNumber, ErrorType::MISS_EQ);
	return false;
}

bool Parser::statement_id_exp()
{
	move();

	if (token.tokenClass == "identifier")
	{
		return true;
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
			createError(token.lineNumber, ErrorType::MISS_OP);
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
			createError(token.lineNumber, ErrorType::MISS_OP);
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
			createError(token.lineNumber, ErrorType::MISS_OP);
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
	move();
	if (token.tokenClass == "identifier")
	{
		move();
		LOG("identifier")
		if (local_var_end())
		{
			LOG("type identifier ;")
			return true;
		}
		else if (local_var_init())
		{
			LOG("INIT")
			move();
			if (local_var_end())
			{
				LOG("type identifier = op ;")
				return true;
			}
			else if (local_var_list())
			{
				LOG("type identifier = op ,")
				local_var();
			}
		}
		else if (local_var_list())
		{
			local_var();
		}
	}

	createError(token.lineNumber, ErrorType::MISS_ID);
	return false;
}

bool Parser::local_var_end()
{
	if (token.lexeme == ";")
	{
		return true;
	}

	return false;
}

bool Parser::local_var_init()
{
	if (token.lexeme == "=")
	{
		move();
		if (token.tokenClass == "digit" || token.tokenClass == "identifier")
		{
			return true;
		}
	}
	return false;
}

bool Parser::local_var_list()
{
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
