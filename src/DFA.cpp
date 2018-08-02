/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：DFA.cpp
 ** 创建人：zeya
 ** 创建日期： 2018-07-28
 **
 ** 描  述：确定性有穷自动机
 ** 注  意：
 **************************************************/
#include <set>
#include <stack>
#include <vector>
#include <cstdio>

#include "DFA.h"

using namespace std;

int DFA::build()
{
    DState *dstate = new DState();

    // start dstate
    closure(nfa.bigStates[0], dstate->nfaStates);
    dstates[&dstate->nfaStates] = dstate;

    //

    return 0;
}

int DFA::init()
{
    return 0;
}

int DFA::closure(NFA::State *state, BitSet &bitset)
{
    set<int> mark;
    stack<NFA::State*> stateStack;

    stateStack.push(state);
    while (!stateStack.empty()) {

        if (mark.find(state->seq) != mark.end()) {
            // 已经标记
            continue;
        }

        bitset.set(state->seq);
        mark.insert(state->seq);
        for (vector<NFA::Edge>::iterator it = state->vec.begin();
             it != state->vec.end();
             ++it) {
            if (it->value.empty()) {
                stateStack.push(it->next);
            }
        }
    }
    return 0;
}

void DFA::printDState(DState *dstate)
{
    printf("{");
    for (long i = 0; i < dstate->nfaStates.num; i++) {
        if (dstate->nfaStates.check(i)) {
            printf("%ld,", i);
        }
    }
    printf("}");
}
