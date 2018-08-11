/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：LexGen.cpp
 ** 创建人：zeya
 ** 创建日期： 2018-07-21
 **
 ** 描  述：词法分析器生成器
 ** 注  意：
 **************************************************/
#include <iostream>
#include <cassert>
#include <stack>
#include <string>
#include <cstdio>
#include <fstream>
#include "LexGen.h"
using namespace std;

int LexGen::init(Conf *confs, int conflen)
{
    int seq = 1;
    // 算序号
    for (int i = 0; i < conflen; i++) {
        seq = (confs[i].end > seq) ? (confs[i].end + 1): seq;
    }

    nfa.init(seq);
    for (int i = 0; i < conflen; i++) {
        nfa.add(confs[i].start, confs[i].end, confs[i].regexp,
                confs[i].isfinish);
    }
    dfa.build();
    genTable();
    // nfa.debugPrint();
    // dfa.print();
    return 0;
}

void LexGen::genTable()
{
    DFA::DState *dstate = dfa.start;
    stack<DFA::DState*> stateStack;
    set<int> mark;

    stateTable = (int(*)[257])new char[sizeof(int) * 257 * (dfa.numStates + 1)];

    for (int i = 0; i < 256; i++) {
        stateTable[0][i] = dfa.start->seq;
    }
    stateTable[0][256] = 0;

    stateStack.push(dstate);
    while (!stateStack.empty()) {
        dstate = stateStack.top();
        stateStack.pop();
        assert(dstate != NULL);

        if (mark.find(dstate->seq) != mark.end()) {
            continue;
        }
        mark.insert(dstate->seq);

        for (int i = 0; i < 256; i++) {
            if (dstate->next[i] == NULL) {
                stateTable[dstate->seq][i] = 0;
                continue;
            }
            stateTable[dstate->seq][i] = dstate->next[i]->seq;
            stateStack.push(dstate->next[i]);
        }
        if (dstate->isfinal) {
            stateTable[dstate->seq][256] = findEndState(dstate);
        }
    }
}

/**
 * 此处与DFA的有些重复，
 * 这个深度遍历做的次数过多
 * 要在第二版中考虑改进
 * 0非终结状态，终结状态>0
 */
int LexGen::findEndState(DFA::DState *dstate)
{
    // NOTE: 只选取第一个终结状态
    int  state = 0;

    for (int i = 0; i < dstate->nfaStates.num; i++) {
        if (!dstate->nfaStates.check(i)) {
            continue;
        }
        // 找每个nfa状态
        BitSet bs(dstate->nfaStates.num);

        dfa.closure(i, bs, -1);
        for (int j = 0; j < bs.num; j++) {
            if (!bs.check(j)) {
                continue;
            }
            // 找每个nfa状态的闭包
            if (nfa.endStates.find(j) !=
                nfa.endStates.end()) {
                // 找闭包内的状态是否是终结状态
                state = j;
                goto finished;
            }
        }

    }
finished:
    return state;
}

int LexGen::match(const char *str, int len, Res *r)
{
    int  curr = 0;
    int  lastType = 0;
    int  lastFinal = 0;

    curr = DFA_INIT_SEQ;
    for (int i = 0; i < len; i++) {
        char c = str[i];

        curr = stateTable[curr][(unsigned)c];
        if (curr == 0) {
            // error or finish
            break;
        }
        if (stateTable[curr][256] > 0) {
            lastFinal = i;
            lastType  = stateTable[curr][256];
        }
    }

    // match failure
    if (lastFinal == 0) {
        return -1;
    }
    // match success
    if (r) {
        r->size = lastFinal + 1;
        r->type = lastType;
    }
    return 0;
}

int LexGen::printTable()
{
    fprintf(stdout, "static int stateTable[%d][257] = ",
            dfa.numStates + 1);
    cout << "{" << endl;
    for (int i = 0; i < dfa.numStates + 1; i++) {
        cout << "    {";
        for (int j = 0; j < 257; j++) {
            cout << stateTable[i][j];
            if (j != 256) {
                cout << ",";
            }
        }
        cout << "}";
        if (i != dfa.numStates) {
            cout << ",";
        }
        cout << endl;
    }
    cout << "};" << endl;
    return 0;
}

int LexGen::printCode()
{
    string line;
    ifstream r("example.cpp", ios::in);

    while (getline(r, line)) {
        cout << line << endl;
    }
    return 0;
}
