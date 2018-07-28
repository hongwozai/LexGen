#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "NFA.h"
using namespace std;

TEST(nfa, read)
{
    NFA nfa;
    string regexp = "abc[e(fg]|ert";
    string regexp1 = "(abc|efg[a-z])uy(er|23|bfsd)";
    string regexp2 = "a*";
    string regexp3 = "def[abc]*";
    string regexp4 = "[abc]?";
    string regexp5 = "[abc]+";
    string regexp6 = "def(abc|dce)?";
    string regexp7 = "def(abc|dce)+";
    string regexp8 = "def(abc|dce)*";
    nfa.init();
    // EXPECT_EQ(0, nfa.read(regexp1.data(), regexp1.size()));
    // EXPECT_EQ(0, nfa.read(regexp.data(), regexp.size()));
    // EXPECT_EQ(0, nfa.read(regexp2.data(), regexp2.size()));
    // EXPECT_EQ(0, nfa.read(regexp3.data(), regexp3.size()));
    // EXPECT_EQ(0, nfa.read(regexp4.data(), regexp4.size()));
    // EXPECT_EQ(0, nfa.read(regexp5.data(), regexp5.size()));
    // EXPECT_EQ(0, nfa.read(regexp6.data(), regexp6.size()));
    EXPECT_EQ(0, nfa.read(regexp7.data(), regexp7.size()));
    // EXPECT_EQ(0, nfa.read(regexp8.data(), regexp8.size()));
    nfa.debugPrint();
}