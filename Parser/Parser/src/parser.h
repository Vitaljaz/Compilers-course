#pragma once
#include "lexer.h"

#include <stack>

#define LOG(X) std::cout << X << std::endl;

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
	BR_MISS_PAIR,
	BAD_STAT_START,
	ERR_MISS_START,
};

struct Error
{
	unsigned line;
	std::string errorMessage;
	ErrorType errorType;
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
	Token lastId;
	Token lastDigit;
	Token lastOperator;

	Token prevToken;
	Token token;

	bool lastSign = false;
	bool needMove = false;

	bool forthird = false;

	std::ofstream asmOut;

	std::vector<Token> tokenList;
	std::vector<Error> errorsList;

	std::stack<Brackets> bracketsList;

	// asm 
	std::map<std::string, int> regID;
	std::vector<Token> expressionTokens;
	std::vector<Token> for2, for3;
	std::stack<int> constructions;
	std::vector<int> forLabels;

	int regCounter = 0;
	int labelCounter = 0;
	int forLabelCounter = 1;

	int forLevel = 0;

	bool for_2();
	bool for_3();
	void createFor2Construction();
	void createFor3Construction();
	void createForJump();
	void createForLabel();
	void createStatement();
	void createCompareIf();
	void createLabel();
	void createInitVar(int digit = 1);
	void createEqual();
	void createUnary();
	//

	void createError(unsigned line, ErrorType errorNumber);
	void getLexems();
	void move();

	void statements();
	bool statement();

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

	bool checkBrackets();
};