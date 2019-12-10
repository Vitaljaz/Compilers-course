#include "parser.h"

#include <clocale>


Parser::Parser(const std::string & fileName_)
{
	lexer = Lexer(fileName_);
	asmOut.open("asm_output.txt");
}

void Parser::run()
{
	getLexems();
	prevToken.st = SymbolType::NONE;
	lexer.printLexemList();
	token = tokenList.front();
	statements();
}

void Parser::createError(unsigned line, ErrorType errorNumber)
{
	if (!errorsList.empty())
	{
		Error tmp = errorsList.back();
		if (tmp.line == line)
			return;
	}

	setlocale(LC_ALL, "Russian");
	switch (errorNumber)
	{
	case ErrorType::ERR_MISS_START:
		errorsList.push_back({ line,"������ ������ ���������.\n", ErrorType::ERR_MISS_START });
		break;
	case ErrorType::MISS_ID:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " �������� 'ID'.\n", ErrorType::MISS_ID});
		break;
	case ErrorType::MISS_EQ:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme +  " �������� ���� '='.\n", ErrorType::MISS_EQ });
		break;
	case ErrorType::MISS_OP:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " �������� 'OPERAND'.\n", ErrorType::MISS_OP });
		break;
	case ErrorType::MISS_BR_O:
		errorsList.push_back({ line,"�������� ������ ������ ��������� '('.\n", ErrorType::MISS_BR_O });
		break;
	case ErrorType::MISS_BR_C:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " �������� ���� ')'.\n", ErrorType::MISS_BR_C });
		break;
	case ErrorType::MISS_END_SEP:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " �������� ���� ';'.\n", ErrorType::MISS_END_SEP });
		break;
	case ErrorType::MISS_F_BR_O:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " �������� ���� '{'.\n", ErrorType::MISS_F_BR_O });
		break;
	case ErrorType::MISS_F_BR_C:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " �������� ���� '}'.\n", ErrorType::MISS_F_BR_C });
		break;
	case ErrorType::DOUBLE_UNARY:
		errorsList.push_back({ line,"� ��������� ������� ��������� ������� ��������.\n", ErrorType::DOUBLE_UNARY });
		break;
	case ErrorType::EXP_START:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " ������ ���� ������ ��������� 'OP'.\n", ErrorType::EXP_START });
		break;
	case ErrorType::EXP_MISS_OP:
		errorsList.push_back({ line,"����������� ����� ���������, �������� ������ ';' ��� '=', ��� ','.\n", ErrorType::EXP_MISS_OP });
		break;
	case ErrorType::FOR_MISS_START:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " ������ ���� ������ ���������.\n", ErrorType::FOR_MISS_START });
		break;
	case ErrorType::EXP_MISS_OP_LIST:
		errorsList.push_back({ line,"�������� 'ID' ����� ������� ',' .\n", ErrorType::EXP_MISS_OP_LIST });
		break;
	case ErrorType::EXP_MISS_INIT_ARG:
		errorsList.push_back({ line,"����� '=' �������� �������� 'ID' ��� 'DIGIT' .\n", ErrorType::EXP_MISS_INIT_ARG });
		break;
	case ErrorType::EXP_MISS_NEXT_ARG:
		errorsList.push_back({ line,"����� 'operand' �������� ������ ';' ��� ','.\n", ErrorType::EXP_MISS_NEXT_ARG });
		break;
	case ErrorType::MISS_START_EXP:
		errorsList.push_back({ line,"��������� ������ ��������� ����� '('.\n", ErrorType::MISS_START_EXP });
		break;
	case ErrorType::MISS_WHAT_EXP:
		errorsList.push_back({ line,"��������� ����������� ���������. ������ ����� ���: (expression).\n", ErrorType::MISS_WHAT_EXP });
		break;
	case ErrorType::BR_MISS_PAIR:
		errorsList.push_back({ line,"����������� ������ ������ ��� }.\n", ErrorType::MISS_WHAT_EXP });
		break;
	case ErrorType::BAD_STAT_START:
		errorsList.push_back({ line,"�� ����� ������� " + token.lexeme + " ������ ���� ������ ���������.\n", ErrorType::MISS_WHAT_EXP });
		break;
	case ErrorType::ST_EXP_MISS_OP:
		errorsList.push_back({ line, "�������� ������� � ��������� ���������.\n", ErrorType::ST_EXP_MISS_OP });
		break;
	}
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
	errorsList.clear();

	while (!tokenList.empty())
			statement();

	if (!checkBrackets())
		createError(prevToken.lineNumber, ErrorType::BR_MISS_PAIR);

	if (errorsList.empty())
	{
		std::cout << "OK! Good grammar!\n";
		asmOut.close();
	}
	else
	{
		std::cout << "NO! Errors:\n";
		for (auto& it : errorsList)
			std::cout << "[Line " << it.line << "]: " << it.errorMessage;
		asmOut.close();
	}
}

bool Parser::statement()
{
	if (needMove)
		move();

	needMove = true;

	if (token.tokenClass == "type")
	{
		if (local_var())
			return true;
	}
	else if (token.lexeme == "if")
	{
		constructions.push(1);
		if (br_open())
		{
			if (expression())
			{
				createCompareIf();
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
				if (for_2())
				{
					forthird = true;
					if (for_3())
					{
						createFor2Construction();
						forthird = false;
						if (statement())
						{
							createFor3Construction();
							return true;
						}
						else
						{
							createError(token.lineNumber, ErrorType::BAD_STAT_START);
							return false;
						}
					}
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_BR_O);
			return false;
		}
	}
	else if (token.lexeme == "while")
	{
		if (br_open())
		{
			if (expression())
			{
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
	else if (token.lexeme == "break")
	{
		if (end_lexeme())
		{
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
		bracketsList.push(Brackets::O_BR);
		if (statement())
			return true;
		else
			return false;
	}
	else if (token.lexeme == "}")
	{
		if (!constructions.empty())
		{
			createLabel();
			constructions.pop();
		}
		if (prevToken.lexeme == ")")
		{
			createError(token.lineNumber, ErrorType::ERR_MISS_START);
			return false;
		}
		if (bracketsList.empty())
		{
			if (tokenList.size() == 0)
				return false;

			createError(token.lineNumber, ErrorType::BR_MISS_PAIR);
			return false;
		}
		else
		{
			if (bracketsList.top() == Brackets::O_BR)
			{
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
	else if (token.tokenClass == "identifier")
	{
		if (prevToken.lexeme == ";" || prevToken.lexeme == "}" || prevToken.lexeme == "{")
		{
			lastId = token;
			expressionTokens.push_back(token);
			if (statement_exp_start())
			{
				expressionTokens.push_back(token);
				if (statement_exp())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_END_SEP);
			return false;
		}
	}
	else if (token.lexeme == "else")
	{
		if (prevToken.lexeme == "}")
		{
			if (statement())
			{
				return true;
			}
			else
			{
				createError(token.lineNumber, ErrorType::ERR_MISS_START);
				return false;
			}
		}
		else
		{
			createError(token.lineNumber, ErrorType::MISS_F_BR_C);
			return false;
		}
	}
	else if ((prevToken.lexeme == ";" || prevToken.lexeme == "}" || prevToken.lexeme == "{") && token.tokenClass == "unary operator")
	{
		lastOperator = token;
		move();
		if (token.tokenClass == "identifier")
		{
			expressionTokens.push_back(token);
			if (statement_exp_start())
			{
				createUnary();
				if (token.lexeme == "=")
				{
					expressionTokens.push_back(token);
				}

				if (statement_exp())
					return true;
				else
					return false;
			}
			else
			{
				createError(token.lineNumber, ErrorType::MISS_END_SEP);
				return false;
			}
		}
	}
	else if (prevToken.lexeme != ";" || prevToken.lexeme != "}")
	{
		if (!tokenList.empty())
		{
			createError(token.lineNumber, ErrorType::ERR_MISS_START);
			return false;
		}
	}
	return false;
}

bool Parser::for_opt()
{
	move();
	if (token.tokenClass == "type")
	{
		if (local_var())
			return true;
	}
	else if (token.tokenClass == "identifier")
	{
		move();
		if (token.lexeme == "=")
		{
			if (statement_exp())
				return true;
		}
	}

	createError(token.lineNumber, ErrorType::FOR_MISS_START);
	return false;
}


bool Parser::for_2()
{
	move();

	if (token.tokenClass == "identifier")
	{
		for2.push_back(token);
		move();
		if (token.tokenClass == "relational operator")
		{
			for2.push_back(token);
			move();
			if (token.tokenClass == "identifier" || token.tokenClass == "digit")
			{
				for2.push_back(token);
				move();
				if (token.lexeme == ";")
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Parser::for_3()
{
	move();

	if (token.tokenClass == "unary operator")
	{
		for3.push_back(token);
		move();
		if (token.tokenClass == "identifier")
		{
			for3.push_back(token);
			move();
			if (token.lexeme == ")")
			{
				return true;
			}
		}
	}

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
				return true;
		}
		else
		{
			createError(token.lineNumber, ErrorType::DOUBLE_UNARY);
			return false; 
		}
	}
	else if (token.tokenClass == "identifier" || token.tokenClass == "digit"
			|| token.lexeme == "true" || token.lexeme == "false") // need operand
	{
		expressionTokens.push_back(token);
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
	if (token.lexeme == ";")
	{
		if (!expressionTokens.empty())
		{
			createStatement();
		}
		return true;
	}

	move();

	if (token.tokenClass == "identifier" || token.tokenClass == "digit"
		|| token.lexeme == "true" || token.lexeme == "false") // operand
	{
		if (lastOperator.tokenClass == "unary operator")
		{
			createUnary();
		}

		expressionTokens.push_back(token);
		lastDigit = token;
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
	createError(token.lineNumber, ErrorType::ST_EXP_MISS_OP);
	return false;
}

bool Parser::what_statement_exp()
{
	move();
	if (token.tokenClass == "logical operator")
	{
		expressionTokens.push_back(token);
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
	else if (token.tokenClass == "relational operator")
	{
		expressionTokens.push_back(token);
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
		expressionTokens.push_back(token);
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
		if (expressionTokens.size() == 3)
		{
			createEqual();
		}
		else
		{
			createStatement();
		}
		return true;
	}
	else if (token.lexeme == ")")
	{
		return true;
	}

	return false;
}

bool Parser::statement_exp_start()
{
	move();
	
	if (token.lexeme == "=")
		return true;
	else if (token.lexeme == ";")
		return true;

	createError(token.lineNumber, ErrorType::MISS_EQ);
	return false;
} // check = or ;

bool Parser::statement_id_exp()
{
	move();

	if (token.tokenClass == "identifier")
		return true;

	return false;
}

bool Parser::what_expression()
{
	move();
	
	if (token.tokenClass == "logical operator")
	{
		expressionTokens.push_back(token);
		if (expression())
			return true;
		else 
			return false;
	}
	else if (token.tokenClass == "relational operator")
	{
		expressionTokens.push_back(token);
		if (expression())
			return true;
		else
			return false;
	}
	else if (token.tokenClass == "arithmetic operator")
	{
		expressionTokens.push_back(token);
		if (expression())
			return true;
		else
			return false;
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
		LOG("Come to local var")
		lastId = token;

		if (forthird)
		{
			if (br_close())
			{
				return true;
			}
			else
			{
				token = prevToken;
			}
		}

		move();

		if (local_var_end())
		{
			createInitVar(0);
			return true;
		}
		else if (local_var_init())
		{
			move();
			if (token.tokenClass == "digit" || token.tokenClass == "identifier")
			{
				lastDigit = token;

				move();
				if (local_var_end())
				{
					createInitVar();
					return true;
				}
				else if (local_var_list())
				{
					createInitVar();
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
			createInitVar(0);

			if (local_var())
				return true;
			else
				return false;
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
		return true;

	return false;
}

bool Parser::local_var_init()
{
	if (token.lexeme == "=")
		return true;

	return false;
}

bool Parser::local_var_list()
{
	if (token.lexeme == ",")
		return true;

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

// asm

void Parser::createFor3Construction()
{
	Token sg = for3.front();
	for3.erase(for3.begin());
	Token id = for3.front();
	for3.erase(for3.begin());

	asmOut << "\n#unary exp:\n";
	asmOut << "mov eax, " << lastId.lexeme << "\n";

	if (sg.lexeme == "++")
	{
		asmOut << "add eax, 1\n";
	}
	else if (sg.lexeme == "--")
	{
		asmOut << "sub eax, 1\n";
	}
	else if (sg.lexeme == "!")
	{
		asmOut << "add eax, not\n"; // ???
	}

	asmOut << "mov " << lastId.lexeme << ", eax\n";
	asmOut << "jmp " << "$FOR_LABEL" << forLabels.back() + 1 << "\n";
	asmOut << "$FOR_LABEL" << forLabels.back() + 3 << ":\n";
}

void Parser::createFor2Construction()
{
	Token op1 = for2.front();
	for2.erase(for2.begin());
	Token sign = for2.front();
	for2.erase(for2.begin());
	Token op2 = for2.front();
	for2.erase(for2.begin());

	if (forLabels.size() == 0)
	{
		forLabels.push_back(1);
	}
	else
	{
		forLabels.push_back(forLabels.back() + 1);
	}

	asmOut << "\n#create for cmp:\n";
	asmOut << "$FOR_LABEL" << forLabels.back() + 1 << ":\n";
	asmOut << "cmp " << op1.lexeme << ", " << op2.lexeme << "\n";
	if (sign.lexeme == "<")
	{
		asmOut << "jle " << "$FOR_LABEL" << forLabels.back() + 3 << "\n";
	}
	else if (sign.lexeme == ">")
	{
		asmOut << "jge " << "$FOR_LABEL"  << forLabels.back() + 3 << "\n";
	}
	else if (sign.lexeme == "==")
	{
		asmOut << "jne " << "$FOR_LABEL"  << forLabels.back() + 3 << "\n";
	}
	else if (sign.lexeme == "!=")
	{
		asmOut << "je " << "$FOR_LABEL"  << forLabels.back() + 3 << "\n";
	}

}

void Parser::createForJump()
{
	if (forLevel == 1)
	{
		asmOut << "jmp $FOR_LABEL" << forLabelCounter + 3 << ":\n";
		forLabelCounter++;
	}
	else if (forLevel == 2)
	{
		asmOut << "jmp $FOR_LABEL" << forLabelCounter + 1 << ":\n";
		forLabelCounter++;
	}
}

void Parser::createForLabel()
{
	if (forLevel == 2)
	{
		std::cout << forLabelCounter;
		asmOut << "$FOR_LABEL" << forLabelCounter  << ":\n";
	}
}

void Parser::createStatement()
{
	Token main = expressionTokens.front();
	expressionTokens.erase(expressionTokens.begin());

	Token eq = expressionTokens.front();
	expressionTokens.erase(expressionTokens.begin());

	std::cout << main.lexeme << "  " << eq.lexeme << std::endl;

	if (eq.lexeme == "=" && main.tokenClass == "identifier")
	{
		asmOut << "\n#create statement:\n";
		asmOut << "mov " << "eax" << ", [";

		for (auto& i : expressionTokens)
			asmOut << " " << i.lexeme << " ";

		asmOut << "]\n";
		asmOut << "mov " << main.lexeme << ", " << "eax" << "\n";
	}

	expressionTokens.clear();
}

void Parser::createCompareIf()
{
	Token op1 = expressionTokens.front();
	expressionTokens.erase(expressionTokens.begin());
	Token sign = expressionTokens.front();
	expressionTokens.erase(expressionTokens.begin());
	Token op2 = expressionTokens.front();
	expressionTokens.erase(expressionTokens.begin());

	asmOut << "\n#create if cmp:\n";
	asmOut << "cmp " << op1.lexeme << ", " << op2.lexeme << "\n";
	if (sign.lexeme == "<")
	{
		asmOut << "jle " << "$LABEL" << labelCounter << "\n";
	}
	else if (sign.lexeme == ">")
	{
		asmOut << "jge " << "$LABEL" << labelCounter << "\n";
	}
	else if (sign.lexeme == "==")
	{
		asmOut << "jne " << "$LABEL" << labelCounter << "\n";
	}
	else if (sign.lexeme == "!=")
	{
		asmOut << "je " << "$LABEL" << labelCounter << "\n";
	}
	labelCounter++;
}

void Parser::createLabel()
{
	asmOut <<"$LABEL" << constructions.size() - 1 << ":\n";
}

void Parser::createInitVar(int digit)
{
	asmOut << "\n#create var:\n";
	if (digit == 0)
	{
		asmOut << "mov " << lastId.lexeme << ", " << 0 << "\n";
	}
	else
	{
		asmOut << "mov " << lastId.lexeme << ", " << lastDigit.lexeme << "\n";
	}
}

void Parser::createEqual()
{
	asmOut << "\n#equal exp:\n";
	if (lastDigit.tokenClass == "identifier")
	{
		asmOut << "mov " << "eax, " << lastDigit.lexeme << "\n";
		asmOut << "mov " << lastId.lexeme << ", eax\n";
	}
	else
	{
		asmOut << "mov " << lastId.lexeme << ", " << lastDigit.lexeme << "\n";
	}
}

void Parser::createUnary()
{
	asmOut << "\n#unary exp:\n";
	asmOut << "mov eax, " << lastId.lexeme << "\n";
	
	if (lastOperator.lexeme == "++")
	{
		asmOut << "add eax, 1\n";
	}
	else if (lastOperator.lexeme == "--")
	{
		asmOut << "sub eax, 1\n";
	}
	else if (lastOperator.lexeme == "!")
	{
		asmOut << "add eax, not\n"; // ???
	}

	asmOut << "mov " << lastId.lexeme << ", eax\n";
}