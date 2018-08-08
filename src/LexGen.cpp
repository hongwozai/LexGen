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
#include <string>
#include "LexGen.h"
using namespace std;


void LexGen::genTable()
{
    nfa.init();
    nfa.reserve(10);
    nfa.add(0, 2, "<.*>");
    nfa.add(0, 3, "\\w+");
    nfa.debugPrint();
    // dfa.build();

    // string html = "<html><body>word</body></html>";
    // dfa.search(html.data(), html.size());
}