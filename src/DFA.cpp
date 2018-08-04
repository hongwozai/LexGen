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
 * @return -1错误状态，0走了正常状态
 */
int DFA::closure(NFA::State *start, BitSet &bitset, int c)
{
    stack<NFA::State*> stateStack;
    stack<NFA::State*> cStack;
    NFA::State *state = start;
    vector<int> vec;
    BitSet tempset(bitset.num);

    // 1. 找闭包
    stateStack.push(state);
    while (!stateStack.empty()) {
        state = stateStack.top();
        stateStack.pop();

        if (tempset.check(state->seq)) {
            continue;
        }
        tempset.set(state->seq);
        cStack.push(state);
        for (vector<NFA::Edge>::iterator it = state->vec.begin();
             it != state->vec.end();
             ++it) {
            if (it->value.empty()) {
                stateStack.push(it->next);
            }
        }
    }

    if (c < 0 || c > 255) {
        return 1;
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
            if (it->val[c] == true) {
                vec.push_back(it->next->seq);
                bitset.set(it->next->seq);
            }
        }
    }
    if (vec.empty()) {
        // 走到错误状态了,应该将set的位全部消除
        return -1;
    }
    return 0;
}

void DFA::printDState(BitSet &set)
{
    printf("{");
    for (long i = 0; i < set.num; i++) {
        if (set.check(i)) {
            printf("%ld,", i);
        }
    }
    printf("}");
}

int DFA::nextDState(DState &dstate, int c, BitSet &set)
{
    int errnum = 0;
    int setnum = 0;
    map<int, NFA::State*>::iterator it;

    for (int i = 0; i < nfa.numStates; i++) {
        if (!dstate.nfaStates.check(i)) {
            continue;
        }
        it = nfa.bigStates.find(i);
        if (it == nfa.bigStates.end()) {
            assert(!"dstate must be contains the state");
        }

        setnum++;
        // dstate所包含的NFA::State
        if (-1 == closure(it->second, set, c)) {
            // 走c会遇到错误状态(无可达状态)
            errnum ++;
        }
    }
    if (errnum >= setnum) {
        return -1;
    }
    return 0;
}

int DFA::build()
{
    stack<DState*> dStack;
    DState *dstate = new DState(nfa.numStates);
    DState *newd = NULL;

    // 遍历
    dstate->nfaStates.set(0);
    dStack.push(dstate);
    while (!dStack.empty()) {
        dstate = dStack.top();
        dStack.pop();

        assert(dstate != NULL);
        if (dstates.find(&dstate->nfaStates) != dstates.end()) {
            continue;
        }
        dstates[&dstate->nfaStates] = dstate;

        cout << "dstate: ";
        printDState(dstate->nfaStates);
        cout << endl;

        // 遍历字符集
        for (int i = 0; i < 256; i++) {
            if (newd == NULL) {
                newd = new DState(nfa.numStates);
            } else {
                newd->nfaStates.clear();
            }

            if (-1 == nextDState(*dstate, i, newd->nfaStates)) {
                // 下一个dstate找不到了（遇到的全部是nfa状态）
                dstate->next[i] = NULL;
                continue;
            }

            printf("newd: '%c'%x: ", (char)i, i);
            printDState(newd->nfaStates);
            cout << endl;

            // 检查是否存在
            map<BitSet*, DState*, PtrBitSet>::iterator it;
            it = dstates.find(&newd->nfaStates);
            if (it == dstates.end()) {
                dstate->next[i] = newd;
                dStack.push(newd);
                cout << "new !" << endl;
                newd = NULL;
            } else {
                cout << "old" << endl;
                dstate->next[i] = it->second;
            }
        }
        cout << dstates.size() << endl;
        map<BitSet*, DState*, PtrBitSet>::iterator it;
        cout << dstates.size() << ": ";
        for (it = dstates.begin(); it != dstates.end(); ++it) {
            printDState(*it->first);
            cout << "|";
        }
        cout << endl;
    }
    return 0;
}
