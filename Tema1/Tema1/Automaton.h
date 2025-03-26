#pragma once
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
class Automaton
{
protected:
	std::unordered_set<std::string> m_states;
	std::unordered_set<char> m_alphabet;
	std::string m_initialState;
	std::unordered_map<std::string, std::unordered_map<char, std::unordered_set<std::string>>> m_transitionFunctions;
	std::unordered_set<std::string> m_finalStates;
public:
	Automaton() = default;
	/*Automaton(
		std::unordered_set<std::string> states,
		std::unordered_set<char> alphabet,
		std::string initialState,
		std::unordered_map<std::pair<std::string, char>, std::string, PairHash> transitionFunctions,
		std::unordered_set<std::string> finalStates);*/

	Automaton(char symbol, int& counter);
	static Automaton concatenate(const Automaton& a1, const Automaton& a2);
	static Automaton alternate(const Automaton& a1, const Automaton& a2, int& counter);
	static Automaton kleeneStar(const Automaton& a, int& counter);
	static Automaton quantify(const Automaton& a, int& counter);
	static Automaton createNFA(const std::string& polishForm);

	std::unordered_set<std::string> lambdaClosure(const std::string& state) const;
	Automaton convertToDFA(const Automaton& nfa) const;
	std::unordered_map<std::string, std::string> createDFAStatesFromNFA(const Automaton& nfa, std::queue<std::unordered_set<std::string>>& newStates, int& newcounter, Automaton& dfa) const;
	void processTransitionsAndFinalStates(const Automaton& nfa, std::queue<std::unordered_set<std::string>>& newStates, std::unordered_map<std::string, std::string>& allStates, int& newcounter, Automaton& dfa) const;
	std::unordered_set<std::string> getTransitionStates(const std::unordered_set<std::string>& states, char c) const;

	static void printDFA(const Automaton& automaton,std::ofstream& fout);
	bool verifyAutomaton(const Automaton& dfa) const;
	void checkWord(const std::string& word) const;
	static Automaton buildDFA(const std::string& expr);

	friend std::ostream& operator<<(std::ostream& os, const Automaton& automaton);
};


