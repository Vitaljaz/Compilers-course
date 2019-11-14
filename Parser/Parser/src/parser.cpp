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

void Parser::getLexems()
{
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
		LOG("-> type")
		local_var_decl();
	}
	else if (token.lexeme == "if")
	{
		LOG("-> if")
			move();
		if (token.lexeme == "(")
		{
			LOG("-> if -> (")
				expression();
			LOG("-> if (exp) -> statement")
				statement();
		}
		else
		{
			LOG("ERROR: wait ( after if")
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

	}
	else if (token.lexeme == "continue")
	{

	}
	else if (token.lexeme == ";")
	{
		LOG("-> ; -> GOOD")
	}
	else if (token.lexeme == "{")
	{
	}
}

void Parser::expression()
{
	move();
	if (token.tokenClass == "unary")
	{
		LOG("-> unary")
		expression();
	}
	else if (token.tokenClass == "identifier" || token.tokenClass == "digit"
			|| token.lexeme == "true" || token.lexeme == "false")
	{
		if (lastSign || prevToken.lexeme == "(" || prevToken.tokenClass == "unary")
		{
			lastSign = false;
			LOG("-> [L_S] | ( | unary -> OPERAND")
			what_expression();
		}
		else
		{
			LOG("ERROR: incorrect start expression")
		}
	}
}

void Parser::what_expression()
{
	move();
	if (token.lexeme == ")")
	{
		LOG("-> OPERAND -> )")
		return;
	}
	else if (token.tokenClass == "relational")
	{
		lastSign = true;
		expression();
	}
	else if (token.tokenClass == "logical")
	{
		lastSign = true;
		expression();
	}
	else
	{
		LOG("ERROR: wait ) or OPERATION")
	}
}

void Parser::local_var_decl()
{
	move();

	if (token.tokenClass == "identifier")
	{
		LOG("type -> ID")
		move();
		if (token.lexeme == ";")
		{
			LOG("type -> ID -> ;")
			return;
		}
		else if (token.lexeme == "=")
		{
			LOG("type -> ID -> =")
			move();
			if (token.tokenClass == "digit" || token.tokenClass == "identifier")
			{
				LOG("type -> ID -> = -> OP")
				move();
				if (token.lexeme == ",")
				{
					LOG("type -> ID -> = -> OP -> ,")
					local_var_decl();
				}
				else if (token.lexeme == ";")
				{
					return;
				}
			}
		}
		else if (token.lexeme == ",")
		{
			local_var_decl();
		}
	}
}
