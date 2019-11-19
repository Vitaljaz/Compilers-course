#include "parser.h"

#include <clocale>


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
	if (!errorsList.empty())
	{
		Error tmp = errorsList.back();
		if (tmp.line == line)
		{
			return;
		}
	}

	setlocale(LC_ALL, "Russian");
	switch (errorNumber)
	{
	case ErrorType::MISS_ID:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " пропущен 'ID'.\n", ErrorType::MISS_ID});
		break;
	case ErrorType::MISS_EQ:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme +  " пропущен знак '='.\n", ErrorType::MISS_EQ });
		break;
	case ErrorType::MISS_OP:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " пропущен 'OPERAND'.\n", ErrorType::MISS_OP });
		break;
	case ErrorType::MISS_BR_O:
		errorsList.push_back({ line,"Пропущен символ начала выражения '('.\n", ErrorType::MISS_BR_O });
		break;
	case ErrorType::MISS_BR_C:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " пропущен знак ')'.\n", ErrorType::MISS_BR_C });
		break;
	case ErrorType::MISS_END_SEP:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " пропущен знак ';'.\n", ErrorType::MISS_END_SEP });
		break;
	case ErrorType::MISS_F_BR_O:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " пропущен знак '{'.\n", ErrorType::MISS_F_BR_O });
		break;
	case ErrorType::MISS_F_BR_C:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " пропущен знак '}'.\n", ErrorType::MISS_F_BR_C });
		break;
	case ErrorType::DOUBLE_UNARY:
		errorsList.push_back({ line,"В выражении имеется повторный унарный оператор.\n", ErrorType::DOUBLE_UNARY });
		break;
	case ErrorType::EXP_START:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " должно быть начало выражения 'OP'.\n", ErrorType::EXP_START });
		break;
	case ErrorType::EXP_MISS_OP:
		errorsList.push_back({ line,"Неожиданный конец выражения, ожидался символ ';' или '=', или ','.\n", ErrorType::EXP_MISS_OP });
		break;
	case ErrorType::FOR_MISS_START:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " должно быть начало выражения.\n", ErrorType::FOR_MISS_START });
		break;
	case ErrorType::EXP_MISS_OP_LIST:
		errorsList.push_back({ line,"Ожидался 'ID' после символа ',' .\n", ErrorType::EXP_MISS_OP_LIST });
		break;
	case ErrorType::EXP_MISS_INIT_ARG:
		errorsList.push_back({ line,"После '=' ожидался аргумент 'ID' или 'DIGIT' .\n", ErrorType::EXP_MISS_INIT_ARG });
		break;
	case ErrorType::EXP_MISS_NEXT_ARG:
		errorsList.push_back({ line,"После 'operand' ожидался символ ';' или ','.\n", ErrorType::EXP_MISS_NEXT_ARG });
		break;
	case ErrorType::MISS_START_EXP:
		errorsList.push_back({ line,"Пропущено начало выражения после '('.\n", ErrorType::MISS_START_EXP });
		break;
	case ErrorType::MISS_WHAT_EXP:
		errorsList.push_back({ line,"Пропущено продолжение выражения. Должно иметь вид: (expression).\n", ErrorType::MISS_WHAT_EXP });
		break;
	case ErrorType::BR_MISS_PAIR:
		errorsList.push_back({ line,"Отсутствует парная скобка для }.\n", ErrorType::MISS_WHAT_EXP });
		break;
	case ErrorType::BAD_STAT_START:
		errorsList.push_back({ line,"На месте лексемы " + token.lexeme + " должно быть начало выражения.\n", ErrorType::MISS_WHAT_EXP });
		break;
	}
}

void Parser::getLexems()
{
	lexer.printLexemList();
	tokenList = lexer.getTokenList();
}

void Parser::move()
{
	prevToken = token;
	if (!tokenList.empty())
	{
		tokenList.erase(tokenList.begin());
		token = tokenList.front();
	}
	else
	{
		LOG("End tokens\n")
	}
}

void Parser::statements()
{
	errorsList.clear();

	while (!tokenList.empty())
	{
		statement();
	}

	if (!checkBrackets())
	{
		createError(prevToken.lineNumber, ErrorType::BR_MISS_PAIR);
	}

	if (errorsList.empty())
	{
		std::cout << "OK! Good grammar!\n";
	}
	else
	{
		std::cout << "NO! Errors:\n";
		for (auto& it : errorsList)
		{
			std::cout << "[Line " << it.line << "]: " << it.errorMessage;
		}
	}
}

bool Parser::statement()
{
	if (first)
		move();

	first = true;

	if (token.tokenClass == "type")
	{
		if (local_var())
		{
			return true;
		}
	}
	else if (token.lexeme == "if")
	{
		if (br_open())
		{
			if (expression())
			{
				LOG("CONSTRUCTION if (expression) - done")
					if (statement())
					{
						return true;
					}
					else
					{
						createError(token.lineNumber, ErrorType::BAD_STAT_START);
						return false;
					}
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_BR_O);
			return false;
		}
	}
	else if (token.lexeme == "for")
	{
		if (br_open())
		{
			if (for_opt())
			{
			}
			else
			{

			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_BR_O);
		}
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
				return true;
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
			return false;
		}
	}
	else if (token.lexeme == "continue")
	{
		if (end_lexeme())
		{
			LOG("continue; - good lexeme")
				return true;
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
			return false;
		}
	}
	else if (token.lexeme == ";")
	{
		return true;
	}
	else if (token.lexeme == "{")
	{
		LOG("{")
		bracketsList.push(Brackets::O_BR);
		if (statement())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (token.lexeme == "}")
	{
		if (bracketsList.empty())
		{
			createError(token.lineNumber, ErrorType::BR_MISS_PAIR); // not find pair for bracket
		}
		else
		{
			if (bracketsList.top() == Brackets::O_BR)
			{
				LOG("{ statement } - done")
				bracketsList.pop();
				return true;
			}
			else
			{
				bracketsList.push(Brackets::C_BR);
				return true;
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
				return true;
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
			return false;
		}
	}
	return false;
}

bool Parser::for_opt()
{
	move();

	if (local_var())
	{
		return true;
	}
	else if (statement_exp_start())
	{
		statement_exp();
		return true;
	}

	createError(token.lineNumber, ErrorType::FOR_MISS_START);
	return false;
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
			createError(token.lineNumber, ErrorType::MISS_WHAT_EXP);
			return false;
		}
	}
	else if (prevToken.lexeme == "(")
	{
		createError(token.lineNumber, ErrorType::MISS_START_EXP);
		return false;
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
			if (token.tokenClass == "digit" || token.tokenClass == "identifier")
			{
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
					return true;
				}
				else
				{
					createError(token.lineNumber, ErrorType::EXP_MISS_NEXT_ARG);
					return false;
				}
			}
			else
			{
				createError(token.lineNumber, ErrorType::EXP_MISS_INIT_ARG);
				return false;
			}
		}
		else if (local_var_list())
		{
			if (local_var())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::EXP_MISS_OP);
			return false;
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
		return true;
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

bool Parser::checkBrackets()
{
	if (bracketsList.empty())
		return true;

	return false;
}