#pragma once
#include "lexer.h"

#include <stack>

#define LOG(X) std::cout << X << std::endl;

struct Error
{
	unsigned line;
	std::string errorMessage;
	ErrorType errorType;
};

enum class ErrorType : int
{
	MISS_ID = 0,
	MISS_EQ, // =
	MISS_OP, // int float id
	MISS_BR_O,
	MISS_BR_C,
	MISS_WHAT_EXP,
	MISS_START_EXP,
	MISS_END_SEP,
	MISS_F_BR_O,
	MISS_F_BR_C,
	DOUBLE_UNARY,
	EXP_START,
	EXP_MISS_OP,
	EXP_MISS_OP_LIST,
	EXP_MISS_INIT_ARG,
	EXP_MISS_NEXT_ARG,
	ST_EXP_MISS_OP,
	FOR_MISS_START,
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

	void createError(unsigned line, ErrorType errorNumber);
	void getLexems();
	void move();

	void statements();
	void statement();

	bool expression();
	bool what_expression();

	bool local_var();
	bool local_var_init();
	bool local_var_end();
	bool local_var_list();

	bool statement_id_exp();
	bool statement_exp();
	bool statement_exp_start();
	bool what_statement_exp();
	
	bool for_opt();

	bool end_lexeme();

	bool br_open();
	bool br_close();
};