#include "lexer.h"

#include <QFile>
#include <QMessageBox>
#include <QMessageBox>
#include <QTextStream>

Lexer::Lexer(const QString& fileName_)
{
	fileName = fileName_;
	initializeHashMap();
	runAnalysis();
}

std::vector<Token>& Lexer::getTokenList()
{
	return tokenList;
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
    keywords.insert(std::pair<std::string, std::string>("char", "keyword"));
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
    additional.insert(std::pair<std::string, std::string>(",", "separator"));
    additional.insert(std::pair<std::string, std::string>("_", "separator"));
    additional.insert(std::pair<std::string, std::string>("+", "arithmetic operator"));
    additional.insert(std::pair<std::string, std::string>("-", "arithmetic operator"));
    additional.insert(std::pair<std::string, std::string>("=", "arithmetic operator"));
    additional.insert(std::pair<std::string, std::string>("*", "arithmetic operator"));
    additional.insert(std::pair<std::string, std::string>("/", "arithmetic operator"));
    additional.insert(std::pair<std::string, std::string>("%", "arithmetic operator"));
    additional.insert(std::pair<std::string, std::string>("<", "relational operator"));
    additional.insert(std::pair<std::string, std::string>(">", "relational operator"));
    additional.insert(std::pair<std::string, std::string>(">=", "relational operator"));
    additional.insert(std::pair<std::string, std::string>("<=", "relational operator"));
    additional.insert(std::pair<std::string, std::string>("!=", "relational operator"));
    additional.insert(std::pair<std::string, std::string>("==", "relational operator"));
    additional.insert(std::pair<std::string, std::string>("&&", "logical operator"));
    additional.insert(std::pair<std::string, std::string>("&", "bitwise operator"));
    additional.insert(std::pair<std::string, std::string>("|", "bitwise operator"));
    additional.insert(std::pair<std::string, std::string>("||", "logical operator"));
    additional.insert(std::pair<std::string, std::string>("!", "unary operator"));
    additional.insert(std::pair<std::string, std::string>("++", "unary operator"));
    additional.insert(std::pair<std::string, std::string>("--", "unary operator"));
}

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
    else if (it->second == "unary operator")
    {
        return SymbolType::U_OPERATOR;
    }
    else if (it->second == "bitwise operator")
    {
        return SymbolType::B_OPERATOR;
    }

	return SymbolType();
}

std::string Lexer::getLexemeClass(const std::string& lexeme)
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

void Lexer::createToken(std::string& t_lexeme, unsigned int line, SymbolType type)
{
	if (machineState != MachineState::NONE)
	{
		if (machineState == MachineState::SIGN)
		{
			std::string LType = getLexemeClass(t_lexeme);
			Token token = { t_lexeme, LType, line };
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

void Lexer::runAnalysis()
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return;
    }

    QTextStream in(&file);

    while (!in.atEnd())
	{
		line.clear();
		lexeme.clear();

		lexemeType = SymbolType::NONE;
		charType = SymbolType::NONE;
		machineState = MachineState::NONE;

        QString t_text = in.readLine();
        line = t_text.toLocal8Bit().constData();

		if (line.size() == 0)
			continue;

		for (size_t i = 0; i <= line.size(); ++i)
		{
			if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n')
			{
				if (lexemeType != SymbolType::NONE)
				{
					createToken(lexeme, lineCounter, lexemeType);
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

            if (machineState == MachineState::SIGN && (line[i] == '+' || line[i] == '-'))
            {
                if ((lexeme == "+" && line[i] == '+') || (lexeme == "-" && line[i] == '-'))
                {
                    lexeme += line[i];
                    machineState = MachineState::NONE;
                    createToken(lexeme, lineCounter, lexemeType);
                    lexemeType = SymbolType::NONE;
                    lexeme.clear();
                    continue;
                }
            }

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
            else if (lexemeType == SymbolType::LETTER && charType == SymbolType::DIGIT)
            {

                lexeme += line[i];
                lexemeType = SymbolType::LETTER;
                continue;
            }
            else if ((lexemeType == charType) && (charType == SymbolType::DIGIT))
            {
                lexeme += line[i];
                continue;
            }
            else if ((lexemeType == charType) && (charType == SymbolType::LETTER))
            {
                lexeme += line[i];
                continue;
            }
            else if (lexeme == "=" && line[i] == '=')
            {
                lexeme += line[i];
                createToken(lexeme, lineCounter, lexemeType);
                lexemeType = SymbolType::NONE;
                lexeme.clear();
                continue;
            }
            else if ((lexemeType == SymbolType::B_OPERATOR) && (charType == SymbolType::B_OPERATOR))
            {
                if ((lexeme == "&" && line[i] == '&') || (lexeme == "|" && line[i] == '|'))
                {
                    lexeme += line[i];
                    createToken(lexeme, lineCounter, lexemeType);
                    lexemeType = SymbolType::NONE;
                    lexeme.clear();
                    continue;
                }
            }
            else if ((lexemeType == SymbolType::U_OPERATOR || lexemeType == SymbolType::R_OPERATOR) && line[i] == '=')
            {
                lexeme += line[i];
                createToken(lexeme, lineCounter, lexemeType);
                lexemeType = SymbolType::NONE;
                lexeme.clear();
                continue;
            }

            if (lexemeType == charType || lexemeType != charType)
            {
                createToken(lexeme, lineCounter, lexemeType);
                lexeme.clear();
                lexeme += line[i];
                lexemeType = charType;
                continue;
            }
        }
        lineCounter++;
	}
    file.close();
}
