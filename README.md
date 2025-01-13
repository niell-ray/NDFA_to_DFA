NFA-to-DFA Conversion in C

This repository contains a C program that converts a Non-deterministic Finite Automaton (NFA) to a Deterministic Finite Automaton (DFA) using the subset (powerset) construction method. The code demonstrates:
	•	How to read NFA specifications (states, transitions, accepting states)
	•	How to represent sets of states as bitmasks
	•	How to systematically build DFA states via a BFS-like approach
	•	How to handle the trap (empty) state as a real DFA state
	•	How to print the resulting DFA in a readable transition table format

Features
	1.	Bitmask Representation
	•	Each subset of NFA states is stored as an integer bitmask (e.g., if state 0 is in the subset, bit 0 is set).
	•	Subset {0, 2} would be stored as 1 << 0 | 1 << 2 = 0b0101.
	2.	Trap/Empty State Support
	•	When no valid transition is found for a subset on a given input symbol, the program creates or reuses an empty set (trap) subset in the DFA.
	•	Ensures the resulting DFA is “total” (every state has a valid transition on every symbol).
	3.	Clear Output
	•	Prints each DFA state (subset of original NFA states).
	•	Marks the start state and accepting states.
	•	Prints a transition table that shows (CurrentDFA, InputSymbol) -> NextDFA.

How It Works
	1.	Input
	•	Prompts for the number of NFA states, the size of the input alphabet, and the number of transitions.
	•	Reads each transition in the format:

 fromState inputSymbol toState

 •	Reads how many accepting states there are, then which states are accepting.

	2.	Construction
	•	Uses a BFS-like approach to discover all reachable subsets:
	1.	Start from the subset containing just the start state (0).
	2.	For each subset, for each symbol, union the transitions of all states in that subset.
	3.	If the resulting subset (bitmask) has not been seen before, add it as a new DFA state.
	•	Special-case: if the resulting subset is 0 (no states), it is treated as the trap state.
	3.	Output
	•	Lists the discovered DFA states (numbered in the order found).
	•	Indicates which ones are accepting (if they contain any NFA accepting state).
	•	Labels which is the start state (the first discovered subset).
	•	Prints a transition table with rows for each DFA state and columns for each symbol.

Provide Input as prompted. Example:

Enter the number of NFA states (0..31): 3
Enter the number of input symbols: 2
Enter the number of transitions: 4
Enter each transition in the form: fromState inputSymbol toState
(Example: 0 a 1)
0 a 1
0 b 0
1 b 2
2 a 2
Enter the number of accepting states in the NFA: 1
Enter the accepting states (space-separated): 2

Example Output

Sample output (your spacing may vary slightly):

=== DFA STATES (subset of NFA states) ===
DFA State 0 = { 0 } [Start]
DFA State 1 = { 1 }
DFA State 2 = { } [Trap]
DFA State 3 = { 2 } [Accepting]

=== DFA TRANSITION TABLE ===
CurrentDFA | Input | NextDFA
--------------------------------
     0      |   a   |   1
     0      |   b   |   0
     1      |   a   |   2
     1      |   b   |   3
     2      |   a   |   2
     2      |   b   |   2
     3      |   a   |   3
     3      |   b   |   2


In this example:
	•	DFA State 2 is the trap state (the empty set).
	•	DFA State 3 is accepting because it contains NFA state 2, which is accepting.

Customizing
	•	Number of States
	•	The code handles up to 32 NFA states by default. You can increase MAX_STATES if you want more (but you might also need to adjust the BFS subset array if you exceed 10 states).
	•	Input Alphabet
	•	By default, assumes symbols are 'a', 'b', 'c', ...' in sequence. Modify symIndex = sym - 'a' and related lines if your input symbols differ.
	•	Epsilon-Transitions
	•	This code does not handle \varepsilon-moves out of the box. You would need to compute \varepsilon-closures for each subset if your NFA has \varepsilon-transitions.

Contributing

Feel free to open pull requests to improve:
	•	Handling of larger alphabets
	•	Support for \varepsilon-transitions
	•	Additional test examples and automated test suites

 
