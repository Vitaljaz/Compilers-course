#include "lexer.h"

SymbolType Lexer::getType(const char ch)
{
	if (isdigit(ch))
	{
		return SymbolType::DIGIT;
	}
	else if (isalpha(ch))
	{
		return SymbolType::LETTER;
	}

	std::string tmp{ ch };
	auto it = additional.find(tmp);

	if (it->second == "separator")
	{
		return SymbolType::SEPARATOR;
	}
	else if (it->second == "arithmetic operator")
	{
		return SymbolType::A_OPERATOR;
	}
	else if (it->second == "relational operator")
	{
		return SymbolType::R_OPERATOR;
	}
	else if (it->second == "logical operator")
	{
		return SymbolType::L_OPERATOR;
	}

	return SymbolType();
}

std::string Lexer::getLexemeClass(std::string lexeme)
{
	auto it = keywords.find(lexeme);
	if (it != keywords.end())
	{
		return it->second;
	}
	
	if ((it = additional.find(lexeme)) != additional.end())
	{
		return it->second;
	}

	return "identifier";
}

void Lexer::printLexemList()
{
	std::cout << "Lexems: " << tokenList.size() << std::endl;
	for (auto it : tokenList)
		std::cout << it.lineNumber << " - " << it.lexeme << " - " << it.tokenClass << std::endl;
}

bool Lexer::isDFA(const char ch)
{
	if (machineState == MachineState::NONE && charType == SymbolType::DIGIT)
	{
		machineState = MachineState::DIGIT;
		return true;
	}

	if (charType == SymbolType::A_OPERATOR)
	{
		if (machineState == MachineState::NONE && (ch == '+' || ch == '-'))
		{
			machineState = MachineState::SIGN;
			return true;
		}
		else if (machineState == MachineState::E && (ch == '+' || ch == '-'))
		{
			machineState = MachineState::L_DIGIT;
			return true;
		}
	}
	
	if (machineState != MachineState::NONE)
	{
		if (machineState == MachineState::SIGN && charType == SymbolType::DIGIT)
		{
			machineState = MachineState::DIGIT;
			return true;
		}
		else if (machineState == MachineState::DIGIT &&  charType == SymbolType::DIGIT)
		{
			return true;
		}
		else if (machineState == MachineState::DIGIT && ch == '.')
		{
			machineState = MachineState::DOT;
			return true;
		}
		else if (machineState == MachineState::DOT && charType == SymbolType::DIGIT)
		{
			machineState = MachineState::M_DIGIT;
			return true;
		}
		else if (machineState == MachineState::M_DIGIT && charType == SymbolType::DIGIT)
		{
			return true;
		}
		else if (machineState == MachineState::M_DIGIT && ch == 'e')
		{
			machineState = MachineState::E;
			return true;
		}
		else if (machineState == MachineState::E && (ch == '+' || ch == '-'))
		{
			machineState = MachineState::L_SIGN;
			return true;
		}
		else if (machineState == MachineState::L_SIGN && charType == SymbolType::DIGIT)
		{
			machineState = MachineState::L_DIGIT;
			return true;
		}
		else if (machineState == MachineState::L_DIGIT && charType == SymbolType::DIGIT)
		{
			return true;
		}
		else if (machineState == MachineState::L_DIGIT && charType != SymbolType::DIGIT)
		{
			machineState = MachineState::END;
			return false;
		}
	}
	return false;
}

void Lexer::initializeHashMap()
{
	initializeAdditional();
	initializeKeyWords();
}

void Lexer::initializeKeyWords()
{
	keywords.insert(std::pair<std::string, std::string>("if", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("for", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("while", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("break", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("continue", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("int", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("double", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("float", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("bool", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("true", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("false", "keyword"));
	keywords.insert(std::pair<std::string, std::string>("return", "keyword"));
}

void Lexer::initializeAdditional()
{
	additional.insert(std::pair<std::string, std::string>("{", "separator"));
	additional.insert(std::pair<std::string, std::string>("}", "separator"));
	additional.insert(std::pair<std::string, std::string>("(", "separator"));
	additional.insert(std::pair<std::string, std::string>(")", "separator"));
	additional.insert(std::pair<std::string, std::string>(";", "separator"));
	additional.insert(std::pair<std::string, std::string>(".", "separator"));
	additional.insert(std::pair<std::string, std::string>("_", "separator"));
	additional.insert(std::pair<std::string, std::string>("+", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("-", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("=", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("*", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("/", "aritmetic operator"));
	additional.insert(std::pair<std::string, std::string>("%", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("<", "relational operator"));
	additional.insert(std::pair<std::string, std::string>(">", "relational operator"));
	additional.insert(std::pair<std::string, std::string>(">=", "relational operator"));
	additional.insert(std::pair<std::string, std::string>("<=", "relational operator"));
	additional.insert(std::pair<std::string, std::string>("!=", "relational operator"));
	additional.insert(std::pair<std::string, std::string>("!", "relational operator"));
	additional.insert(std::pair<std::string, std::string>("==", "relational operator"));
	additional.insert(std::pair<std::string, std::string>("&&", "logical operator"));
	additional.insert(std::pair<std::string, std::string>("&", "logical operator"));
	additional.insert(std::pair<std::string, std::string>("|", "logical operator"));
	additional.insert(std::pair<std::string, std::string>("||", "logical operator"));
	
}

void Lexer::runAnalysis()
{
	std::ifstream fin;
	fin.open(fileName);

	if (!fin.is_open())
	{
		std::cout << "File not found.\n";
		return;
	}

	while (!fin.eof())
	{	
		line.clear();
		lexeme.clear();

		lexemeType = SymbolType::NONE;
		charType  = SymbolType::NONE;
		machineState = MachineState::NONE;

		getline(fin, line);

		if (line.size() == 0)
			continue;

		for (size_t i = 0; i <= line.size(); ++i)
		{
			if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n')
			{
				if (lexemeType != SymbolType::NONE)
				{
					createToken(lexeme, lineCounter, charType);
					lexeme.clear();
					lexemeType = SymbolType::NONE;

					if (machineState != MachineState::NONE)
						machineState = MachineState::NONE;

					continue;
				}
				continue;
			}

			if (line[i] == '/' && lexeme == "/")
			{
				lexeme.clear();
				lexemeType = SymbolType::NONE;
				break;
			}

			charType = getType(line[i]);

			// need fix
			if (lexemeType == SymbolType::NONE || lexemeType == SymbolType::D_CONST)
			{
				if (isDFA(line[i]))
				{
					if (!lexeme.empty() && machineState == MachineState::SIGN)
					{
						createToken(lexeme, lineCounter, charType);
						lexeme.clear();
					}
					lexemeType = SymbolType::D_CONST;
					lexeme += line[i];
					continue;
				}
				else
				{
					if (machineState != MachineState::NONE)
					{
						createToken(lexeme, lineCounter, charType);
						lexeme.clear();
						lexeme += line[i];
						lexemeType = charType;
						machineState = MachineState::NONE;
						continue;
					}
				}
			}


			if (lexemeType == SymbolType::NONE)
			{
				lexemeType = charType;
				lexeme += line[i];
				continue;
			}

			if (lexemeType == SymbolType::LETTER && charType == SymbolType::DIGIT)
			{
				lexeme += line[i];
				lexemeType = SymbolType::LETTER;
				continue;
			}

			if ((lexemeType == charType) && (charType == SymbolType::DIGIT))
			{
				lexeme += line[i];
				continue;
			}
			else if ((lexemeType == charType) && (charType == SymbolType::LETTER))
			{
				lexeme += line[i];
				continue;
			}

			if (lexemeType != charType)
			{
				createToken(lexeme, lineCounter, charType);
				lexeme.clear();
				lexeme += line[i];
				lexemeType = charType;
				continue;
			}
		}
		lineCounter++;
	}
}

void Lexer::createToken(std::string& t_lexeme, unsigned int line, SymbolType type)
{
	if (machineState != MachineState::NONE)
	{
		if (machineState == MachineState::DIGIT)
		{
			std::string LType = getLexemeClass(t_lexeme);
			Token token = { t_lexeme, "digit", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::DIGIT)
		{
			Token token = { t_lexeme, "digit", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::DOT)
		{
			t_lexeme += "0";
			Token token = { t_lexeme, "digit", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::M_DIGIT)
		{
			Token token = { t_lexeme, "digit", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::E)
		{
			Token token = { t_lexeme, "exponential notation", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::L_SIGN)
		{
			Token token = { t_lexeme, "exponential notation", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::L_DIGIT)
		{
			Token token = { t_lexeme, "exponential notation", line };
			tokenList.push_back(token);
			return;
		}
		else if (machineState == MachineState::END)
		{
			Token token = { t_lexeme, "exponential notation", line };
			tokenList.push_back(token);
			return;
		}
	}

	if (type == SymbolType::DIGIT)
	{
		Token token = { t_lexeme, "digit", line };
		tokenList.push_back(token);
	}
	else
	{
		std::string LType = getLexemeClass(t_lexeme);
		Token token = { t_lexeme, LType, line };
		tokenList.push_back(token);
	}
}

Lexer::Lexer(const std::string& fileName_)
{
	fileName = fileName_;
	initializeHashMap();
	runAnalysis();
}
