#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "NFA.h"
using namespace std;

TEST(nfa, read)
{
    NFA nfa;
    nfa.read("abc", 3);
}