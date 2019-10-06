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
}

void Lexer::initializeAdditional()
{
	additional.insert(std::pair<std::string, std::string>("{", "separator"));
	additional.insert(std::pair<std::string, std::string>("}", "separator"));
	additional.insert(std::pair<std::string, std::string>("(", "separator"));
	additional.insert(std::pair<std::string, std::string>(")", "separator"));
	additional.insert(std::pair<std::string, std::string>(";", "separator"));
	additional.insert(std::pair<std::string, std::string>("+", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("-", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("=", "arithmetic operator"));
	additional.insert(std::pair<std::string, std::string>("*", "arithmetic operator"));
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
		std::string line;
		std::string lexeme;

		SymbolType lexemType = SymbolType::NONE;
		SymbolType charType  = SymbolType::NONE;

		getline(fin, line);

		if (line.size() == 0)
			continue;

		for (size_t i = 0; i <= line.size(); ++i)
		{
			if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n')
			{
				if (lexemType != SymbolType::NONE)
				{
					createToken(lexeme, lineCounter, lexemType);
					lexeme.clear();
					lexemType = SymbolType::NONE;
					continue;
				}
				continue;
			}

			charType = getType(line[i]);

			if (lexemType == SymbolType::NONE)
			{
				lexemType = charType;
				lexeme += line[i];
				continue;
			}

			if (lexemType == SymbolType::LETTER && charType == SymbolType::DIGIT)
			{
				lexeme += line[i];
				lexemType = SymbolType::LETTER;
				continue;
			}

			if ((lexemType == charType) && (charType == SymbolType::DIGIT))
			{
				lexeme += line[i];
				continue;
			}
			else if ((lexemType == charType) && (charType == SymbolType::LETTER))
			{
				lexeme += line[i];
				continue;
			}

			if (lexemType != charType)
			{
				createToken(lexeme, lineCounter, lexemType);
				lexeme.clear();
				lexeme += line[i];
				lexemType = charType;
				continue;
			}
		}

		lineCounter++;
	}
}

void Lexer::createToken(const std::string t_lexeme, int line, SymbolType type)
{
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

Lexer::Lexer(std::string fileName_)
{
	fileName = fileName_;
	initializeHashMap();
	runAnalysis();
}
