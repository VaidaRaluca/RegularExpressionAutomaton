#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include "Automaton.h"

int main()
{
	std::string expression;
	std::ifstream fin("Input.txt");
	std::ofstream fout("Output.txt");
	if (!fin.is_open()) {
		std::cout << "Failed to open input file\n";
		return 1;
	}

	if (!fout.is_open()) {
		std::cout << "Failed to open output file\n";
		return 1;
	}

	fin >> expression;
	if (expression == "") {
		std::cout << "Invalid regex\n";
		return 0;
	}
	Automaton DFA = Automaton::buildDFA(expression);
	if (!DFA.verifyAutomaton(DFA)) {
		std::cout << "Invalid DFA\n";
		return 0;
	}

	char c;
	do {
		std::cout << "Introduce: \na to print the regex \nb to print dfa \nc to check if a word is accepted by dfa\n";
		std::cin >> c;

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (c == 'a') {
			std::cout << expression << '\n';
		}
		else if (c == 'b') {
			Automaton::printDFA(DFA, fout);
		}
		else if (c == 'c') {
			std::cout << "Introduce a word: ";
			std::string word;
			std::cin >> word;
			DFA.checkWord(word);
		}
	} while (c != 'x');

	return 0;
}
