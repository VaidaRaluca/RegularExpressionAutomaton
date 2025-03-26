#include "Automaton.h"
#include <algorithm>

int priority(char c) {
	if (c == '*' || c == '+')
		return 3;
	else if (c == '.')
		return 2;
	else if (c == '|')
		return 1;
	else
		return -1;
}

std::string infixToPostfix(const std::string& s) {
	std::stack<char> st; // stack of symbols
	std::string result = ""; // string in polish form
	for (int i = 0; i < s.length(); i++) {
		char c = s[i];
		// if it is a valid character add it to the string
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
			result += c;
		// if it is an open paranthesis add it to the stack of symbols
		else if (c == '(')
			st.push('(');
		else if (c == ')') {
			// start popping symbols from the stack until (
			while (st.top() != '(') {
				result += st.top(); // add all the symbols between () to the string
				st.pop();
			}
			st.pop(); // pop the closed paranthesis
		}
		else {
			// if its a different symbol like *.|+
			while (!st.empty() && priority(c) <= priority(st.top())) {
				result += st.top(); // add symbols to the string based on their priority
				st.pop();
			}
			st.push(c); // add the symbol as well
		}
	}
	while (!st.empty()) {
		result += st.top(); // add whatever is left behind
		st.pop();
	}
	return result;
}

/// <summary>
/// creates an automaton with a non +*|. symbol
/// </summary>
/// <param name="symbol"></param>
/// <param name="counter"></param>
Automaton::Automaton(char symbol, int& counter) {
	std::string startState = "q" + std::to_string(counter++); // create initial state
	std::string endState = "q" + std::to_string(counter++); // create final state
	m_states = { startState, endState }; // adding the two to the states set
	m_alphabet = { symbol }; // the alphabet is made of just the current symbol
	m_initialState = startState; // initialization
	m_finalStates = { endState }; // initialization
	m_transitionFunctions[startState][symbol].insert(endState); // from start state with current symbol you end up in end state
}

/// <summary>
/// concatenates two automatons into one 
/// </summary>
/// <param name="a1"></param>
/// <param name="a2"></param>
/// <returns></returns>
Automaton Automaton::concatenate(const Automaton& a1, const Automaton& a2) { // basically . symbol
	Automaton result; // resulted automaton

	result.m_states = a1.m_states; // copies all states from a1 to result
	result.m_transitionFunctions = a1.m_transitionFunctions; // copies all functions from a1 to result

	result.m_states.insert(a2.m_states.begin(), a2.m_states.end()); // appends the states form a2 as well
	for (const auto& [state, transitions] : a2.m_transitionFunctions) { // iterates over each state in a2 and its corresponding transition functions
		for (const auto& [symbol, targetStates] : transitions) { // for each transition iterates over the symbols and the states (end states) reachable by that symbol
			result.m_transitionFunctions[state][symbol].insert(targetStates.begin(), targetStates.end()); // appends all states from a2 to result, avoiding duplicates 
		}
	}
	result.m_alphabet = a1.m_alphabet; // copies a1's alphabet into result
	result.m_alphabet.insert(a2.m_alphabet.begin(), a2.m_alphabet.end()); // appends a2's alphabet into result
	result.m_initialState = a1.m_initialState; // attributes to result the initial state of a1
	std::string finalA1 = *a1.m_finalStates.begin(); // stores a1's first final state
	std::string initialA2 = a2.m_initialState; // stores a2's initial state
	std::string mergedState = finalA1 + "=" + initialA2; // stores a merged state
	result.m_states.erase(finalA1); // erases from result a1's first final state
	result.m_states.erase(initialA2); // erases from results a2's initial state
	result.m_states.insert(mergedState); // inserts into result the merged state

	//finalA1 -> mergedState
	for (auto& [state, transitions] : result.m_transitionFunctions) { // erases all connections to finalA1 and connects them to the merged state by inserting the merged state where finalA1 used to be 
		for (auto& [symbol, targetStates] : transitions) {
			if (targetStates.count(finalA1)) { 
				targetStates.erase(finalA1); 
				targetStates.insert(mergedState);
			}
		}
	}
	if (result.m_transitionFunctions.count(initialA2)) { // same but with initialA2
		auto initialTransitions = result.m_transitionFunctions[initialA2];
		result.m_transitionFunctions.erase(initialA2);
		result.m_transitionFunctions[mergedState] = initialTransitions;
	}
	result.m_transitionFunctions[finalA1]['\0'] = { mergedState }; // connects finalA1 to mergedState through a lambda transition
	for (const auto& finalState : a2.m_finalStates) 
		if (finalState == initialA2) 
			result.m_finalStates.insert(mergedState); // in final states switch initialA2 for the merged state
		else 
			result.m_finalStates.insert(finalState); // otherwise simply insert the current final state
	return result;
}

/// <summary>
/// alternates two automatons 
/// </summary>
/// <param name="a1"></param>
/// <param name="a2"></param>
/// <returns></returns>
Automaton Automaton::alternate(const Automaton& a1, const Automaton& a2, int& counter) { // basically | symbol
	Automaton result;
	// similar to concatenation
	result.m_states = a1.m_states; 
	std::string startState = "q" + std::to_string(counter++);
	std::string endState = "q" + std::to_string(counter++);
	result.m_states.insert(a2.m_states.begin(), a2.m_states.end());
	result.m_states.insert({ startState, endState });
	result.m_initialState = startState;
	result.m_finalStates = { endState };
	result.m_alphabet = a1.m_alphabet;
	result.m_alphabet.insert(a2.m_alphabet.begin(), a2.m_alphabet.end());
	result.m_transitionFunctions = a1.m_transitionFunctions;
	for (const auto& [state, transitions] : a2.m_transitionFunctions) 
		for (const auto& [symbol, targetStates] : transitions) 
			result.m_transitionFunctions[state][symbol].insert(targetStates.begin(), targetStates.end());
	// until here
	result.m_transitionFunctions[startState]['\0'] = { a1.m_initialState, a2.m_initialState }; // from start state with lambda both a1's and a2's initial state can be reached
	for (const auto & final : a1.m_finalStates) 
		result.m_transitionFunctions[final]['\0'] = { endState }; // connects every final state from a1 with lambda to the end state
	for (const auto & final : a2.m_finalStates) 
		result.m_transitionFunctions[final]['\0'] = { endState }; // same for a2
	return result;
}

/// <summary>
/// expresses the repetition of a symbol with the condition that the symbol must appear at least once (idk how to explain)
/// </summary>
/// <param name="a1"></param>
/// <param name="a2"></param>
/// <returns></returns>
Automaton Automaton::kleeneStar(const Automaton& a, int& counter) { // basically *
	// similar to the constructor with a symbol 
	Automaton result;
	result.m_states = a.m_states;
	result.m_alphabet = a.m_alphabet;
	std::string startState = "q" + std::to_string(counter++);
	std::string endState = "q" + std::to_string(counter++);
	result.m_states.insert({ startState, endState });
	result.m_initialState = startState;
	result.m_finalStates = { endState };
	// until here
	result.m_transitionFunctions[startState]['\0'] = { a.m_initialState, endState }; // from start state with lambda to both the inital state and the end state; using = to replace other existing lambda transitions
	for (const auto & final : a.m_finalStates)
		result.m_transitionFunctions[final]['\0'] = { a.m_initialState, endState }; // connects every final state with lambda to both the inital state and the end state
	result.m_transitionFunctions.insert(a.m_transitionFunctions.begin(), a.m_transitionFunctions.end()); // appends the transition functions
	return result;
}

/// <summary>
/// expresses the repetition of a symbol with the condition that the symbol must appear at least zero times (idk how to explain)
/// </summary>
/// <param name="a"></param>
/// <param name="counter"></param>
/// <returns></returns>
Automaton Automaton::quantify(const Automaton& a, int& counter) { // basically +
	// almost the same with kleene star
	Automaton result;
	result.m_states = a.m_states;
	result.m_alphabet = a.m_alphabet;
	std::string startState = "q" + std::to_string(counter++);
	std::string endState = "q" + std::to_string(counter++);
	result.m_states.insert(startState);
	result.m_states.insert(endState);

	result.m_initialState = startState;
	result.m_finalStates = { endState };

	// except here
	result.m_transitionFunctions[startState]['\0']= {a.m_initialState} ; // no going back to endState
	for (const std::string & final : a.m_finalStates) {
		result.m_transitionFunctions[final]['\0'].insert(a.m_initialState); // each final state must connect with lambda to the initial state
		result.m_transitionFunctions[final]['\0'].insert(endState); // each final state must connect with lambda to the end state
	}
	for (const auto& [state, transitions] : a.m_transitionFunctions) //  iterates over each state and its corresponding transitions in the automaton
		for (const auto& [symbol, nextStates] : transitions) // iterates through all of the symbols and the nextStates that are reachable from the current state when the current symbol 
			result.m_transitionFunctions[state][symbol].insert(nextStates.begin(), nextStates.end()); // adds them to the result

	return result;
}

Automaton Automaton::createNFA(const std::string& polishForm) {
	int counter = 0;
	std::stack<Automaton> sa;
	for (auto& symbol : polishForm) {
		if (symbol != '*' && symbol != '.' && symbol != '|' && symbol != '+') {
			sa.push(Automaton(symbol, counter));
		}
		else if (symbol == '|') {
			Automaton b = sa.top(); sa.pop();
			Automaton a = sa.top(); sa.pop();
			Automaton result = Automaton::alternate(a, b, counter);
			sa.push(result);
		}
		else if (symbol == '*') {
			Automaton a = sa.top(); sa.pop();
			Automaton result = Automaton::kleeneStar(a, counter);
			sa.push(result);
		}
		else if (symbol == '+') {
			Automaton a = sa.top(); sa.pop();
			Automaton result = Automaton::quantify(a, counter);
			sa.push(result);
		}
		else if (symbol == '.') {
			Automaton b = sa.top(); sa.pop();
			Automaton a = sa.top(); sa.pop();
			Automaton result = Automaton::concatenate(a, b);
			sa.push(result);
		}
	}
	Automaton result;
	if (!sa.empty())
		result = sa.top();
	else
		std::cout << "Invalid Automaton";
	return result;
}

bool Automaton::verifyAutomaton(const Automaton& dfa) const {
	if (dfa.m_states.empty()) return false;
	if (dfa.m_alphabet.empty()) return false;
	if (auto it = std::find(m_states.begin(), m_states.end(), m_initialState); it == m_states.end())
		return false;
	for (const auto& state : m_states) 
		for (const auto& symbol : m_alphabet)
			if (state == std::string(1, symbol))
				return false;
	if (auto it = m_transitionFunctions.find(m_initialState); it == m_transitionFunctions.end() || it->second.empty()) return false;
	for (const auto& [state, transitions] : m_transitionFunctions){
		if (m_states.find(state) == m_states.end()) return false;
		for (const auto& [symbol, finalStates] : transitions) {
			if (m_alphabet.find(symbol) == m_alphabet.end()) return false;
			for (const auto& finalState : finalStates)
				if (m_states.find(finalState) == m_states.end()) return false;
		}
	}
	return true;
}

void Automaton::checkWord(const std::string& word) const {
	std::unordered_set<std::string> currentStates = { m_initialState };
	for (char symbol : word) {
		if (m_alphabet.find(symbol) == m_alphabet.end()) {
			std::cout << "Word " << word << " is NOT accepted. Invalid symbol\n";
			return ;
		}
		std::unordered_set<std::string> nextStates;
		for (const std::string& state : currentStates) {
			auto it = m_transitionFunctions.find(state);
			if (it != m_transitionFunctions.end()) {
				auto itt = it->second.find(symbol);
				if (itt != it->second.end()) 
					nextStates.insert(itt->second.begin(), itt->second.end()); 
			}
		}
		if (nextStates.empty()) {
			std::cout << "Word " << word << " is NOT accepted. No transition found\n";
			return ;
		}
		currentStates = nextStates;
	}
	for(const std::string& state: currentStates)
		if (m_finalStates.find(state) != m_finalStates.end()) {
			std::cout << "Word " << word << " is accepted\n";
			return ;
		}
	std::cout << "Word " << word << " is NOT accepted\n";
	return ;
}

Automaton Automaton::buildDFA(const std::string& expr){
	std::string regex = infixToPostfix(expr);
	Automaton nfa = Automaton::createNFA(regex);
	Automaton dfa = nfa.convertToDFA(nfa);
	return dfa;
}



/// <summary>
/// stores all the lambda transitions
/// </summary>
/// <param name="state"></param>
/// <returns></returns>
std::unordered_set<std::string> Automaton::lambdaClosure(const std::string& state) const {
	std::unordered_set<std::string> closure = { state }; // initializing a state of closures with the given state
	std::stack<std::string> stack; // a stack will be used to perfom a dfs 
	stack.push(state);  // dfs starts from the given stack

	while (!stack.empty()) { // while the "search" is not done
		std::string current = stack.top();
		stack.pop();
		if (m_transitionFunctions.count(current)) { // checks if the current state has any transition functions defined
			const auto& transitions = m_transitionFunctions.at(current);  
			if (transitions.count('\0')) {  // if is's lambda transition
				const auto& targetStates = transitions.at('\0');  
				for (const auto& target : targetStates)  // iterates through thhe states that are reachable with lambda
					if (closure.insert(target).second)  // checks if the state has already been added
						stack.push(target); 
				    // insert takes a pair where the first parameter is the iterator to the element and the second is a bool indicating the successfullness of the insertion
			}
		}
	}
	return closure;  
}

/// <summary>
/// constructs dfa states from nfa states by mapping them
/// </summary>
/// <param name="nfa"></param>
/// <param name="newStates"></param>
/// <param name="newcounter"></param>
/// <param name="dfa"></param>
/// <returns></returns>
std::unordered_map<std::string, std::string> Automaton::createDFAStatesFromNFA(const Automaton& nfa, std::queue<std::unordered_set<std::string>>& newStates, int& newcounter, Automaton& dfa) const {
	std::unordered_map<std::string, std::string> allStates; // map where the key is a string representing a nfa state and the value is a corresponding dfa name

	std::unordered_set<std::string> startClosure = nfa.lambdaClosure(nfa.m_initialState); // calculates the lambda closures of the nfa's initial state
	std::string startKey;
	std::vector<std::string> sortedStates(startClosure.begin(), startClosure.end()); // the lambda closure set is converted to a vector that can be sorted so that the key generated is consistent and unique for the same set of states
	std::sort(sortedStates.begin(), sortedStates.end());
	for (const auto& state : sortedStates) // the sorted states are concatenated into a string with each state separated by ,
		startKey += state + ",";

	std::string startState = "q" + std::to_string(newcounter++) + "'"; 
	allStates[startKey] = startState; // assigns a unique dfa state to the lambda closure of the nfa's initial state
	dfa.m_states.insert(startState); // the state is added to the dfa's states set
	dfa.m_initialState = startState; // dfa's initial state is set to the start state
	newStates.push(startClosure); // allows further processing of nfa states
	return allStates;
}

/// <summary>
/// constructs a dfa's final states as well as its transition functions based on a nfa
/// </summary>
/// <param name="nfa"></param>
/// <param name="newStates"></param>
/// <param name="allStates"></param>
/// <param name="newcounter"></param>
/// <param name="dfa"></param>
void Automaton::processTransitionsAndFinalStates(const Automaton& nfa, std::queue<std::unordered_set<std::string>>& newStates, std::unordered_map<std::string, std::string>& allStates, int& newcounter, Automaton& dfa) const {
	while (!newStates.empty()) { // while there are unhandled states
		auto currentState = newStates.front();
		newStates.pop();
		std::string currentKey; // generates unique key for each state set
		std::vector<std::string> sortedCurrentState(currentState.begin(), currentState.end()); // declares a vector that can be sorted so that the key generated is consistent and unique for the same set of states
		std::sort(sortedCurrentState.begin(), sortedCurrentState.end());
		for (const auto& state : sortedCurrentState)
			currentKey += state + ',';
		std::string currentDFAState = allStates[currentKey]; // stores the current dfa state
		for (const char& symbol : dfa.m_alphabet) { // iterates over dfa's alphabet
			std::unordered_set<std::string> canBeReached = nfa.getTransitionStates(currentState, symbol); // stores all reachable nfa stated for the current dfa state with symbol
			std::unordered_set<std::string> closure;
			for (const auto& state : canBeReached) { // iterates over the reachable states
				auto stateClosure = nfa.lambdaClosure(state); // storea all lambda closures from state
				closure.insert(stateClosure.begin(), stateClosure.end()); 
			}
			if (!closure.empty()) { // checks if there are reachable nfa states includinf lambda transition
				std::string closureKey;
				std::vector<std::string> sortedClosure(closure.begin(), closure.end());
				std::sort(sortedClosure.begin(), sortedClosure.end());
				for (const auto& state : sortedClosure)
					closureKey += state + ',';
				if (allStates.find(closureKey) == allStates.end()) { // checks if closureKey already exists in allStates
					std::string newState = "q" + std::to_string(newcounter++) + "'"; // creates a new dfa state
					allStates[closureKey] = newState; // map the closureKey to the newly created dfa state
					dfa.m_states.insert(newState);
					newStates.push(closure); // the closure set is added to the queue and will be processed further to discover new dfa states
				}
				if (dfa.m_transitionFunctions.count(currentDFAState)) { // checks if the current dfa state already has transitions defined
					auto& transitions = dfa.m_transitionFunctions[currentDFAState];
					if (transitions.count(symbol)) // checks if there is already a transition for the current symbol
						transitions[symbol].insert(allStates[closureKey]); // adds the new state to the set of target states for that symbol
					else 
						dfa.m_transitionFunctions[currentDFAState][symbol] = { allStates[closureKey] }; // creates new transition function from current dfa state with symbol to a set of target states
				}
				else 
					dfa.m_transitionFunctions[currentDFAState][symbol] = { allStates[closureKey] };
			}
		}
		for (const auto& nfaState : currentState) // iterates over each nfa state in the current dfa state set
			if (nfa.m_finalStates.count(nfaState)) { // checks if the nfa state is a final state
				dfa.m_finalStates.insert(currentDFAState); // marks the current dfa state set as final
				break;
			}
	}
}

/// <summary>
/// converts a nfa to dfa
/// </summary>
/// <param name="nfa"></param>
/// <returns></returns>
Automaton Automaton::convertToDFA(const Automaton& nfa) const {
	int newcounter = 0; // number of states in dfa, used to generate unique names
	Automaton dfa;
	dfa.m_alphabet = nfa.m_alphabet; // copies the alphabet from nfa

	std::queue<std::unordered_set<std::string>> newStates; // keeps sets of nfa state; each set represents a possble state in the dfa, which corresponds to a subset of nfa states
	std::unordered_map<std::string, std::string> allStates = createDFAStatesFromNFA(nfa, newStates, newcounter, dfa);
	processTransitionsAndFinalStates(nfa, newStates, allStates, newcounter, dfa);

	return dfa;
}

/// <summary>
/// return the states that can be reached from a given states set
/// </summary>
/// <param name="states"></param>
/// <param name="c"></param>
/// <returns></returns>
std::unordered_set<std::string> Automaton::getTransitionStates(const std::unordered_set<std::string>& states, char c) const {
	std::unordered_set<std::string> result; // initialization of a set that will hold the states that are reachable from the given states set with a given symbol

	for (const std::string& state : states) { // iterates over each state in the given states set
		if (m_transitionFunctions.count(state)) {
			const auto& transitions = m_transitionFunctions.at(state);
			if (transitions.count(c)) {
				result.insert(transitions.at(c).begin(), transitions.at(c).end()); // inserts all the states thhat can be reached from current state with symbol into the result set
			}
		}
	}
	return result;
}

void Automaton::printDFA(const Automaton& automaton, std::ofstream& fout)
{
	std::cout << automaton;
	fout << automaton;
	fout.flush();
}

std::ostream& operator<<(std::ostream& os, const Automaton& automaton){
	os << "States: ";
	for (const auto& state : automaton.m_states) {
		os << state << " ";
	}
	os << "\n";

	os << "Alphabet: ";
	for (const auto& symbol : automaton.m_alphabet) {
		os << symbol << " ";
	}
	os << "\n";
	os << "Initial State: " << automaton.m_initialState << "\n";
	os << "Final States: ";
	for (const auto& finalState : automaton.m_finalStates) {
		os << finalState << " ";
	}
	os << "\n";

	os << "Transition Functions:\n";
	for (const auto& [state, transitions] : automaton.m_transitionFunctions) 
		for (const auto& [symbol, targetStates] : transitions) 
			for (const auto& target : targetStates) 
				os << "(" << state << ", " << symbol << ") -> " << target << "\n";
	return os;
}
