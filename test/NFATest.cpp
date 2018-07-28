#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "NFA.h"
using namespace std;

TEST(nfa, read)
{
    NFA nfa;
    string regexp = "abc[e(fg]|ert";
    string regexp1 = "(abc|def)uy";
    nfa.init();
    EXPECT_EQ(0, nfa.read(regexp1.data(), regexp1.size()));
}