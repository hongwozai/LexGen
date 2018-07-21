/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：NFA.cpp
 ** 创建人：zeya
 ** 创建日期： 2018-07-21
 **
 ** 描  述：NFA生成
 ** 注  意：
 **************************************************/
#include <cctype>
#include <cassert>
#include <stack>
#include <iostream>

#include "NFA.h"

using namespace std;

int NFA::init()
{
    // initial bigState
    bigStates.insert(pair<int, State*>(0, new State));
    // end bigState
    bigStates.insert(pair<int, State*>(-1, new State));
    return 0;
}

int NFA::read(const char *str, size_t len)
{
    return read(str, len, bigStates[0], bigStates[-1]);
}

int NFA::read(const char *str, size_t len, State *start, State *end)
{
    Edge           *lastEdge = NULL;
    stack<FragNode> fragStack;
    stack<int>      parseStack;
    State          *tempState;
    Frag           *tempFrag;
    FragNode       *tempFragNode;

    fragStack.push(FragNode(new Frag(start, end), 0, 0));
    for (size_t i = 0; i < len; i++) {
        char c = str[i];

        switch (c) {
        case '[': {
            if (parseStack.top() == BRACKET) {
                // TODO: 当作正常字符来操作
                break;
            }
            parseStack.push(BRACKET);
            lastEdge = new Edge();
            break;
        }

        case ']': {
            if (parseStack.top() != BRACKET) {
                // 语法错误
                return -1;
            }
            parseStack.pop();
            lastEdge = NULL;

        }

        case '(': {
            if (parseStack.top() != BRACKET) {
                // TODO: 当成正常字符
                break;
            }
            parseStack.push(PAIR);
        }

        case ')': {
        }

        case '*': {
        }

        case '+': {
        }

        case '.': {
        }

        case '?': {
        }

        case '|': {
        }

        case '\\': {
        }

        default:
            // 正常字符
            switch (parseStack.top()) {
            case BRACKET:
            case PAIR:
            case TRANSFER:
            case NORMAL:
                break;
            default:
                assert(!"State is not exist!");
            }
            break;
        }
    }
    debugPrint(fragStack.top().frag);

    // 栈不为空,语法有错误
    if (!parseStack.empty() || fragStack.size() != 1) {
        return -1;
    }
    return 0;
}

void NFA::debugPrint(Frag *frag)
{
    typedef vector<Edge>::iterator It;
    State *state;

    cout << "digraph {" << endl;

    // 节点
    state = frag->start;
    while (state) {
        for (It it = state->vec.begin(); it != state->vec.end(); ++it) {
            cout << "    ";
            if (!it->next) {
                cout << "[!] Error Edge" << it->value;
                assert(!"[!] Error Edge");
            }

            // 当前节点
            cout << (void*)state;
            cout << " -> ";
            // 指向节点
            cout << (void*)it->next;
            cout << " : " << it->value << endl;
        }
    }

    cout << "}" << endl;
}