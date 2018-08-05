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
    int ret = 0;
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
        if (state->seq == 2) {
            ret = 2;
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
        // 0 or 2
        return ret;
    }
    // 2. 找c边
    while (!cStack.empty()) {
        state = cStack.top();
        cStack.pop();

        assert(state != NULL);
        for (vector<NFA::Edge>::iterator it = state->vec.begin();
             it != state->vec.end();
             ++it) {
            if (start->seq == 0 && state->seq == 5 && c == 'c') {
                cout << "mark 5" << endl;
                for (int i = 0; i < 256; i++) {
                    if (it->val[i] == true)
                        cout << i << ": " << it->val[i] << endl;
                }
            }
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
    return ret;
}

int DFA::nextDState(DState &dstate, int c, BitSet &set)
{
    int ret = 0;
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
        if (-1 == (ret = closure(it->second, set, c))) {
            // 走c会遇到错误状态(无可达状态)
            errnum ++;
        } else if (ret == 2) {
            // 该状态是终结状态
            return ret;
        }
    }
    if (errnum >= setnum) {
        return -1;
    }
    return 0;
}

int DFA::build()
{
    int ret = 0;
    stack<DState*> dStack;
    DState *dstate = new DState(nfa.numStates, seq++);
    DState *newd = NULL;
    set<int> mark;

    start = dstate;
    // 遍历
    dstate->nfaStates.set(0);
    dStack.push(dstate);
    while (!dStack.empty()) {
        dstate = dStack.top();
        dStack.pop();

        assert(dstate != NULL);
        if (mark.find(dstate->seq) != mark.end()) {
            continue;
        }
        mark.insert(dstate->seq);

        cout << "dstate: ";
        printDState(dstate->nfaStates, cout);
        cout << endl;

        // 遍历字符集
        for (int i = 0; i < 256; i++) {
            if (newd == NULL) {
                newd = new DState(nfa.numStates, seq++);
            } else {
                newd->nfaStates.clear();
            }

            if (-1 == (ret = nextDState(*dstate, i, newd->nfaStates))) {
                // 下一个dstate找不到了
                dstate->next[i] = NULL;
                continue;
            }
            if (ret == 2) {
                newd->isfinal = true;
                cout << "seq: " << newd->seq << "isfinal: " << (newd->isfinal ? "true": "false") << endl;
            }

            printf("newd: '%c'%x: ", (char)i, i);
            printDState(newd->nfaStates, cout);
            cout << endl;

            // 检查是否存在
            map<BitSet*, DState*, PtrBitSet>::iterator it;
            it = dstates.find(&newd->nfaStates);
            if (it == dstates.end()) {
                dstate->next[i] = newd;
                dstates[&newd->nfaStates] = newd;
                dStack.push(newd);
                cout << "new!" << endl;
                newd = NULL;
            } else {
                dstate->next[i] = it->second;
            }
        }
    }
    return 0;
}

void DFA::printStateTable()
{
    map<BitSet*, DState*, PtrBitSet>::iterator it;
    /* 打印状态表 */
    cout << dstates.size() << ": ";
    for (it = dstates.begin(); it != dstates.end(); ++it) {
        printDState(*it->first);
        cout << "|";
    }
    cout << endl;
}

void DFA::print()
{
    stack<DState*> dStack;
    DState *dstate;
    set<int> mark;

    cerr << "digraph DFA {" << endl;
    dStack.push(start);
    while (!dStack.empty()) {
        dstate = dStack.top();
        dStack.pop();

        if (mark.find(dstate->seq) != mark.end()) {
            continue;
        }

        mark.insert(dstate->seq);

        for (int i = 0; i < 256; i++) {
            if (dstate->next[i] == NULL) {
                continue;
            }
            cerr << "    ";
            /* from */
            cerr << "\"" << dstate->seq << ":";
            printDState(dstate->nfaStates);
            cerr << "\"";

            cerr << " -> ";

            /* to */
            cerr << "\"" << dstate->next[i]->seq << ":";
            printDState(dstate->next[i]->nfaStates);
            cerr << "\"";

            /* label */
            cerr << " [label=\"";
            cerr << (char)i;
            cerr << "\"]";
            cerr << ";" << endl;

            dStack.push(dstate->next[i]);
        }
    }
    cerr << "}" << endl;
}

void DFA::printDState(BitSet &set, std::ostream &out)
{
    out << "{";
    for (long i = 0; i < set.num; i++) {
        if (set.check(i)) {
            out << i << ",";
        }
    }
    out << "}";
}
