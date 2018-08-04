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
#include <iostream>
#include <cassert>

#include "DFA.h"

using namespace std;

int DFA::init()
{
    return 0;
}

/**
 * 算法思路：简单的方式，找空边，找c边，找空边
 * 另一个想法：
 * 对每个节点记录之前是否走过c边（走过的又遇到没走时候，视为没有走过）
 */
int DFA::closure(NFA::State *start, BitSet &bitset, int c)
{
    stack<NFA::State*> stateStack;
    stack<NFA::State*> cStack;
    NFA::State *state = start;

    // 1. 找闭包
    stateStack.push(state);
    while (!stateStack.empty()) {
        state = stateStack.top();
        stateStack.pop();

        if (bitset.check(state->seq)) {
            continue;
        }
        bitset.set(state->seq);
        cStack.push(state);
        for (vector<NFA::Edge>::iterator it = state->vec.begin();
             it != state->vec.end();
             ++it) {
            if (it->value.empty()) {
                stateStack.push(it->next);
            }
        }
    }

    // 2. 找c边
    while (!cStack.empty()) {
        state = cStack.top();
        cStack.pop();

        assert(state != NULL);
        for (vector<NFA::Edge>::iterator it = state->vec.begin();
             it != state->vec.end();
             ++it) {
            assert(it->next != NULL);
            if (it->val[c] == true && !bitset.check(it->next->seq)) {
                stateStack.push(it->next);
            }
        }
    }

    // 3. 找空边
    while (!stateStack.empty()) {
        state = stateStack.top();
        stateStack.pop();

        assert(state != NULL);
        if (bitset.check(state->seq)) {
            continue;
        }
        bitset.set(state->seq);
        for (vector<NFA::Edge>::iterator it = state->vec.begin();
             it != state->vec.end();
             ++it) {
            assert(it->next != NULL);
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

int DFA::build()
{
    DState *dstate = new DState();

    // start dstate
    // closure(nfa.bigStates[0], dstate->nfaStates);
    // dstates[&dstate->nfaStates] = dstate;

    //

    return 0;
}