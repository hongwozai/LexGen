/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：LexGen.h
 ** 创建人：zeya
 ** 创建日期： 2018-07-21
 **
 ** 描  述：词法分析器生成器
 ** 注  意：1.
 **************************************************/
#ifndef LEXGEN_H
#define LEXGEN_H

#include <string>
#include "NFA.h"
#include "DFA.h"

class LexGen
{
public:

    enum {
        DFA_INIT_SEQ = 1,
    };

    struct Conf {
        int start;
        int end;
        std::string regexp;
        bool isfinish;
    };

    struct Res {
        // 单词的类型
        int type;
        // 单词的长度(单词的开始从字符串的开始)
        int size;
    };

public:

    LexGen()
        : stateTable(0),
          dfa(nfa, DFA_INIT_SEQ)
    {}

    int init(Conf *confs, int conflen);

    /**
     * 匹配
     * 返回定义的nfa状态终结状态seq
     */
    int match(const char *str, int len, Res *r);

public:

    /**
     * 打印状态表
     */
    int printTable();
    int printCode();

private:

    void genTable();

    int findEndState(DFA::DState *dstate);

private:

    // 状态表
    // 256个next位，1个终结状态需要返回的状态
    int (*stateTable)[257];

private:

    NFA nfa;

    DFA dfa;
};

#endif /* LEXGEN_H */
