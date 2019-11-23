#include <iostream>
#include "parser.h"

int main()
{
	Parser parser("input.txt");
	parser.run();
	std::cin.get();
}