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

#include "NFA.h"
#include "DFA.h"

class LexGen
{
public:

    LexGen() : dfa(nfa) {}

    int init();

    /**
     * 搜索
     */
    int search(const char *str, int len);

public:

    /**
     * 打印状态表
     */
    int printTable();

    /**
     * 打印搜索流程
     */
    int printSearch();

    void genTable();

private:

private:

    NFA nfa;

    DFA dfa;
};

#endif /* LEXGEN_H */
