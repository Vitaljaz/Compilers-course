#pragma once
#include "lexer.h"

#define LOG(X) std::cout << X << std::endl;

struct Error
{
	unsigned line;
	std::string errorMessage;
};

class Parser
{
public:
	Parser(const std::string& fileName_);

	~Parser() = default;

	Lexer lexer;

	void run();
	
private:
	Token prevToken;
	Token token;

	bool lastSign = false;
	bool first = false;

	std::vector<Token> tokenList;
	std::vector<Error> errorsList;

	void createError(unsigned line, std::string& message);
	void getLexems();
	void move();

	void statements();
	void statement();

	void expression();
	void what_expression();

	void local_var_decl();
};