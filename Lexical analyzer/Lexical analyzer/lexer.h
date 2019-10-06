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
	COMMENT,    // //
	SEPARATOR,  // { } ( ) ;
	R_OPERATOR, // < > >= <= ! ==
	A_OPERATOR, // + - = * 
	L_OPERATOR  // && ||
};

class Lexer
{
private:
	struct Token
	{
		std::string lexeme;
		std::string tokenClass;
		unsigned int lineNumber;
	};

	std::map<std::string, std::string> keywords;
	std::map<std::string, std::string> additional;

	std::vector<Token> tokenList;

	std::string fileName;

	unsigned int lineCounter = 1;

	SymbolType getType(const char ch);
	std::string getLexemeClass(std::string lexeme);
	
	void initializeHashMap();
	void initializeKeyWords();
	void initializeAdditional();
	void runAnalysis();
	void createToken(const std::string t_lexeme, int line, SymbolType type);

public:
	Lexer() = delete;
	Lexer(std::string fileName_);
	void printLexemList();
	
	~Lexer() = default;
};