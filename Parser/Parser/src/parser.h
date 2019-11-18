#pragma once
#include "lexer.h"

#include <stack>

#define LOG(X) std::cout << X << std::endl;

struct Error
{
	unsigned line;
	std::string errorMessage;
};

enum class Brackets : int {O_BR, C_BR};

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

	std::stack<Brackets> bracketsList;

	void createError(unsigned line, unsigned errorNumber);
	void getLexems();
	void move();

	void statements();
	void statement();

	bool expression();
	bool what_expression();

	bool local_var();
	bool local_var_init();
	bool local_var_r();
	bool local_var_end();
	bool local_var_decl();
	bool local_var_list();

	bool statement_id_exp();
	bool statement_exp();
	bool statement_exp_start();
	bool what_statement_exp();
	

	bool end_lexeme();

	bool br_open();
	bool br_close();
};