#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "NFA.h"
#include "DFA.h"
using namespace std;

TEST(dfa, closure)
{
    NFA nfa;
    // string regexp1 = "(a?b*c|ad*c|aac)*";
    string regexp1 = "\\d+";
    // string regexp1 = "abc?de";
    nfa.init(2);
    nfa.add(0, 1, regexp1.data(), regexp1.size());
    // nfa.debugPrint();
    // EXPECT_EQ(12, nfa.numStates);

    DFA dfa(nfa, 1);
    BitSet set(nfa.numStates);
    // EXPECT_EQ(nfa.numStates, set.num);
    // EXPECT_EQ(2, set.bytes);

    // dfa.closure(nfa.bigStates[0], set, 'a');
    // dfa.closure(nfa.bigStates[0], set, -1);

    // for (int i = 0; i < nfa.numStates; i++) {
    //     if (set.check(i)) {
    //         cout << i << endl;
    //     }
    // }

    // dfa.build();

    // string match = "01239abc";
    // dfa.search(match.data(), match.size());
}