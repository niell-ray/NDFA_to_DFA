#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES  32   // Max number of NFA states (adjust if needed)
#define MAX_SYMBOLS 26   // Max number of input symbols (a, b, c, ...)

int main() {
    int nStates, nSymbols, nTransitions;
    int i, j, k;

    // Read the number of NFA states
    printf("Enter the number of NFA states (0..%d): ", MAX_STATES-1);
    scanf("%d", &nStates);

    // Read the number of input symbols in the alphabet
    printf("Enter the number of input symbols: ");
    scanf("%d", &nSymbols);

    // 2D array of bitmasks: nfaTransitions[state][symbolIndex]
    // Each cell is a bitmask representing all possible next states.
    unsigned int nfaTransitions[MAX_STATES][MAX_SYMBOLS];
    memset(nfaTransitions, 0, sizeof(nfaTransitions));

    // Read the number of transitions
    printf("Enter the number of transitions: ");
    scanf("%d", &nTransitions);

    // Read transitions in the form (fromState inputSymbol toState)
    printf("Enter each transition in the form: fromState inputSymbol toState\n");
    printf("(Example: 0 a 1)\n");
    for (i = 0; i < nTransitions; i++) {
        int from, to;
        char sym;
        scanf("%d %c %d", &from, &sym, &to);

        // Convert 'a', 'b', 'c'... to indices 0, 1, 2...
        int symIndex = sym - 'a';
        if (symIndex < 0 || symIndex >= nSymbols) {
            printf("Error: symbol '%c' out of range.\n", sym);
            return 1;
        }
        if (from < 0 || from >= nStates || to < 0 || to >= nStates) {
            printf("Error: invalid state number.\n");
            return 1;
        }

        // Set the bit for 'to' in the transition table
        nfaTransitions[from][symIndex] |= (1U << to);
    }

    // Read the accepting states of the NFA
    int nAccepting;
    printf("Enter the number of accepting states in the NFA: ");
    scanf("%d", &nAccepting);

    // Bitmask for the NFA's accepting states
    unsigned int nfaAcceptMask = 0;
    if (nAccepting > 0) {
        printf("Enter the accepting states (space-separated): ");
        for (i = 0; i < nAccepting; i++) {
            int acc;
            scanf("%d", &acc);
            nfaAcceptMask |= (1U << acc);
        }
    }

    // -------------------------------------------------------------------------
    // BEGIN SUBSET (POWERSET) CONSTRUCTION
    // -------------------------------------------------------------------------

    // We'll store each distinct subset (bitmask) in dfaStates[].
    // dfaMarked[] tells us if we've processed that subset in BFS (0=unmarked,1=marked).
    // The maximum number of subsets for nStates <= 10 is 2^10 = 1024, so we use 1<<10.
    unsigned int dfaStates[1 << 10];
    int dfaMarked[1 << 10];  
    memset(dfaStates, 0, sizeof(dfaStates));
    memset(dfaMarked, 0, sizeof(dfaMarked));

    // dfaTransitionsD[dfaStateIndex][symbolIndex] = index of the next subset
    int dfaTransitionsD[1 << 10][MAX_SYMBOLS];
    memset(dfaTransitionsD, -1, sizeof(dfaTransitionsD));

    // Assume the NFA start state is 0. The start subset in the DFA is {0} => bitmask = 1 << 0 = 1
    unsigned int startSubset = (1U << 0);

    // Initialize BFS queue pointers (front/rear) for the subsets
    int front = 0;
    int nDFAStates = 0;

    // Add start subset
    dfaStates[nDFAStates] = startSubset; 
    nDFAStates++;

    // BFS loop over subsets
    while (front < nDFAStates) {
        unsigned int curSubset = dfaStates[front];
        dfaMarked[front] = 1; // Mark this subset as processed

        // For each input symbol
        for (j = 0; j < nSymbols; j++) {
            // Calculate the union of NFA transitions from all states in curSubset on symbol j
            unsigned int nextSubset = 0;
            for (k = 0; k < nStates; k++) {
                // If NFA state k is in curSubset
                if (curSubset & (1U << k)) {
                    // Union in the bitmask of transitions from k on symbol j
                    nextSubset |= nfaTransitions[k][j];
                }
            }

            // -----------------------------------------------------------------
            // NEW CODE: Treat the empty subset (0) as a TRAP state
            // -----------------------------------------------------------------
            if (nextSubset == 0) {
                // Look up if we've already added the empty set as a subset
                int foundEmpty = -1;
                for (i = 0; i < nDFAStates; i++) {
                    if (dfaStates[i] == 0) {
                        foundEmpty = i;
                        break;
                    }
                }
                // If not, add it now
                if (foundEmpty == -1) {
                    dfaStates[nDFAStates] = 0;  // empty set
                    dfaMarked[nDFAStates] = 0;
                    foundEmpty = nDFAStates;
                    nDFAStates++;
                }
                // Transition to the empty set state
                dfaTransitionsD[front][j] = foundEmpty;
            } else {
                // If nextSubset != 0, see if we've already discovered it
                int found = -1;
                for (i = 0; i < nDFAStates; i++) {
                    if (dfaStates[i] == nextSubset) {
                        found = i;
                        break;
                    }
                }
                // If not, add it as a new subset
                if (found == -1) {
                    dfaStates[nDFAStates] = nextSubset;
                    dfaMarked[nDFAStates] = 0;
                    found = nDFAStates;
                    nDFAStates++;
                }
                // Record the transition
                dfaTransitionsD[front][j] = found;
            }
        }
        front++;
    }

    // Identify which of these subsets are accepting
    // A subset is accepting if it intersects with nfaAcceptMask => (subset & nfaAcceptMask) != 0
    int dfaAccepting[1 << 10];
    memset(dfaAccepting, 0, sizeof(dfaAccepting));
    for (i = 0; i < nDFAStates; i++) {
        unsigned int subset = dfaStates[i];
        if ((subset & nfaAcceptMask) != 0) {
            dfaAccepting[i] = 1;
        }
    }

    // -------------------------------------------------------------------------
    // END SUBSET CONSTRUCTION
    // -------------------------------------------------------------------------

    // Print out the DFA states (subsets) we discovered
    printf("\n=== DFA STATES (subset of NFA states) ===\n");
    for (i = 0; i < nDFAStates; i++) {
        unsigned int subset = dfaStates[i];
        printf("DFA State %d = { ", i);

        // Print which NFA states are in this subset
        for (k = 0; k < nStates; k++) {
            if (subset & (1U << k)) {
                printf("%d ", k);
            }
        }
        printf("}");

        if (dfaAccepting[i]) {
            printf(" [Accepting]");
        }
        if (i == 0) {
            printf(" [Start]");
        }
        // If subset == 0, you could label it [Trap] if you want
        if (subset == 0) {
            printf(" [Trap]");
        }
        printf("\n");
    }

    // Print the DFA transition table
    printf("\n=== DFA TRANSITION TABLE ===\n");
    printf("CurrentDFA | Input | NextDFA\n");
    printf("--------------------------------\n");
    for (i = 0; i < nDFAStates; i++) {
        for (j = 0; j < nSymbols; j++) {
            int nextState = dfaTransitionsD[i][j];
            // Convert j to a letter (assumes 'a'+j)
            char sym = (char)('a' + j);
            if (nextState == -1) {
                // Should not happen now, because we handle trap states
                printf("     %3d    |   %c   |   -\n", i, sym);
            } else {
                printf("     %3d    |   %c   |   %d\n", i, sym, nextState);
            }
        }
    }

    return 0;
}
