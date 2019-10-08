#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

enum class SymbolType : int
{
	NONE = 0,   // none
	DIGIT,      // 0 - 9
	LETTER,     // a-z
	D_CONST,    // exp const
	SEPARATOR,  // { } ( ) ;
	R_OPERATOR, // < > >= <= ! ==
	A_OPERATOR, // + - = * 
	L_OPERATOR  // && ||
};

enum class MachineState : int
{
	NONE = 0, // none
	SIGN,     // + -
	DIGIT,    // +(-)0-9
	DOT,      // +(-)0-9.
	M_DIGIT,  // +(-)0-9.0-9
	E,        // +(-)0-9.0-9e
	L_SIGN,   // +(-)0-9.0-9e+(-)
	L_DIGIT,  // +(-)0-9.0-9e+(-)0-9
	END       // finish state
};

class Lexer
{
public:
	Lexer() = delete;
	Lexer(const std::string& fileName_);
	void printLexemList();

	~Lexer() = default;

private:
	struct Token
	{
		std::string lexeme;
		std::string tokenClass;
		unsigned int lineNumber;
	};

	unsigned int lineCounter = 1;

	std::map<std::string, std::string> keywords;
	std::map<std::string, std::string> additional;
	std::vector<Token> tokenList;

	std::string fileName;
	std::string line;
	std::string lexeme;

	SymbolType lexemeType;
	SymbolType charType;

	MachineState machineState = MachineState::NONE;

	const SymbolType& getType(const char ch);
	const std::string& getLexemeClass(const std::string& lexeme);
	
	void runAnalysis();
	void initializeHashMap();
	void initializeKeyWords();
	void initializeAdditional();
	void createToken(std::string& t_lexeme, unsigned int line, SymbolType type);
	bool isDFA(const char ch);
};