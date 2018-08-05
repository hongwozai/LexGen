/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：DFA.h
 ** 创建人：zeya
 ** 创建日期： 2018-07-28
 **
 ** 描  述：确定性有穷自动机
 ** 注  意：
 **************************************************/
#ifndef DFA_H
#define DFA_H

#include <map>
#include <set>
#include <iostream>
#include "NFA.h"
#include "BitSet.h"

class DFA
{
public:

    DFA(NFA &nfa)
        : numStates(0),
          start(0),
          nfa(nfa),
          seq(0)
    {}

    ~DFA() {}

    /**
     * init
     */
    int init();

    int build();

    /**
     * 不完全是求闭包,还包括读一个字符
     * c传递-1就是不读字符,0-255是读取一个字符
     */
    int closure(NFA::State *state, BitSet &set, int c);

    void print();

    void printStateTable();

private:

    struct DState {
        BitSet  nfaStates;
        DState *next[256];
        // 该状态序号
        int seq;
        // 是否为终结状态
        bool isfinal;

        DState(int num, int seq)
            : nfaStates(num),
              seq(seq) {
            memset(next, 0, sizeof(void*) * 256);
        }
    };

private:

    void printDState(BitSet &set, std::ostream &out = std::cerr);

    int  nextDState(DState &, int, BitSet &);

private:

    int numStates;

    DState *start;

    // 根据位图(NFA状态集)查找状态
    std::map<BitSet*, DState*, PtrBitSet> dstates;

    // nfa
    NFA &nfa;

    // 全局唯一序号
    int seq;
};

#endif /* DFA_H */
