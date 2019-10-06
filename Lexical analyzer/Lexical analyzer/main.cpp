#include <iostream>
#include "lexer.h"

int main()
{
	Lexer lexer("input.txt");
	lexer.printLexemList();
	std::cin.get();
}