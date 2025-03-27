# Finite Automata from Regular Expressions in C++

## Overview
This project implements **regular expression processing** by converting a **regular expression (regex) into a Non-deterministic Finite Automaton (NFA)** and then **optimizing it into a Deterministic Finite Automaton (DFA)**. The implementation follows the **formal languages and automata theory** concepts, utilizing **C++ data structures** such as sets, maps, and queues for efficient state transitions.

## Process: From Regex to DFA

### 1. **Converting a Regular Expression to an NFA**
A **regular expression** defines a pattern that can be recognized by a finite automaton. The steps to convert a regex into an **NFA** are:

- **Tokenization of the Regular Expression**  
  The regex is first converted into a **Reverse Polish Notation (RPN)** (postfix notation) to handle operator precedence easily.
  
- **Building NFAs for Basic Operations**  
  The following fundamental **NFA constructions** are used:
  - **Single Symbol**: A simple transition from one state to another.
  - **Concatenation (`.` operator)**: The second NFA follows the first NFA.
  - **Alternation (`|` operator)**: A new start state with epsilon transitions to both NFAs.
  - **Kleene Star (`*` operator)**: Adds loops using epsilon (`λ`) transitions.
  - **Quantification (`+` or `?`)**: Extends the Kleene Star or allows optional transitions.

- **Epsilon Closures**  
  Used to find all states reachable via `λ`-transitions from a given state.

### 2. **Converting NFA to DFA**
Since **NFAs allow multiple transitions for the same input symbol**, we must convert them into a **DFA**, which ensures deterministic transitions. The conversion steps are:

- **Compute Epsilon Closure**  
  Find all states reachable through `λ` transitions.
  
- **Construct DFA States**  
  Each DFA state represents a **set of NFA states** (subset construction method).

- **Process Transitions**  
  For each DFA state, compute the next possible states based on input symbols.

- **Define Final States**  
  If any state in a DFA corresponds to an **NFA final state**, it becomes a DFA final state.

### 3. **Minimizing the DFA (Optimization)**
Once the DFA is built, **redundant states** are merged to optimize performance.

---

## Implementation Details

### **Automaton Class**
The `Automaton` class represents an **NFA or DFA**, supporting operations like **concatenation, alternation, Kleene Star, and conversion to DFA**.

#### **Attributes**
- `m_states`: Set of all states.
- `m_alphabet`: Input alphabet.
- `m_initialState`: The initial state.
- `m_transitionFunctions`: Transition table (maps states and symbols to new states).
- `m_finalStates`: Set of accepting states.

#### **Key Methods**
1. **NFA Construction Methods**
   - `Automaton(char symbol, int& counter)`: Creates a simple NFA for a single character.
   - `concatenate(const Automaton& a1, const Automaton& a2)`: Connects two NFAs sequentially.
   - `alternate(const Automaton& a1, const Automaton& a2, int& counter)`: Implements alternation (choice).
   - `kleeneStar(const Automaton& a, int& counter)`: Adds Kleene Star to repeat patterns.
   - `createNFA(const std::string& polishForm)`: Converts **postfix regex to NFA**.

2. **NFA to DFA Conversion**
   - `lambdaClosure(const std::string& state)`: Finds states reachable via `λ`-transitions.
   - `convertToDFA(const Automaton& nfa)`: Constructs the DFA from the NFA.
   - `createDFAStatesFromNFA()`: Generates DFA states from **NFA state subsets**.
   - `processTransitionsAndFinalStates()`: Defines DFA transitions and final states.
   - `getTransitionStates()`: Retrieves all reachable states on a given symbol.

3. **DFA Operations**
   - `printDFA(const Automaton& automaton, std::ofstream& fout)`: Outputs DFA transitions.
   - `verifyAutomaton(const Automaton& dfa)`: Validates if an input belongs to the DFA language.
   - `checkWord(const std::string& word)`: Tests if a word is accepted by the automaton.
   - `buildDFA(const std::string& expr)`: Main function to build a DFA from a regex.

4. **Overloaded Operators**
   - `operator<<`: Prints the automaton in a readable format.

---
## Learning Outcomes
Building this application allowed me to gain a deeper understanding of formal languages and automata theory by converting regular expressions into NFAs and then into DFAs. I enhanced my algorithmic thinking by implementing state transitions, epsilon closures, and minimization techniques. Additionally, this project improved my proficiency in C++ by utilizing data structures like sets and maps to efficiently construct and optimize finite automata.

##
