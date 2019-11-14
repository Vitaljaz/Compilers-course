#include <iostream>
#include "parser.h"

int main()
{
	Parser parser("input.txt");
	parser.run();
	LOG("RUN")
	std::cin.get();
}