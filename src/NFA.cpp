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
    string          newValue;
    stack<Frag>     fragStack;
    stack<int>      parseStack;

    parseStack.push(NORMAL);
    fragStack.push(Frag(start, end));
    for (size_t i = 0; i < len; i++) {
        char c = str[i];

        switch (c) {
        case '[': {
            if (parseStack.top() == BRACKET) {
                // TODO: 当作正常字符来操作
                fragStack.top().appendNode(c);
                break;
            }
            parseStack.push(BRACKET);
            newValue = '[';
            break;
        }

        case ']': {
            Frag &tempFrag = fragStack.top();

            if (parseStack.top() != BRACKET) {
                // 语法错误
                return -1;
            }
            parseStack.pop();

            newValue += ']';
            tempFrag.appendNode(newValue);
            break;
        }

        case '(': {
            Frag &tempFrag = fragStack.top();
            if (parseStack.top() == BRACKET) {
                newValue += c;
                break;
            } else if (parseStack.top() == TRANSFER) {
                fragStack.top().appendNode(c);
                break;
            }
            parseStack.push(PAIR);
            fragStack.push(Frag(tempFrag.last, tempFrag.end));
            break;
        }

        case ')': {
            Frag &tempFrag = fragStack.top();
            if (parseStack.top() == BRACKET) {
                newValue += c;
                break;
            } else if (parseStack.top() == TRANSFER) {
                fragStack.top().appendNode(c);
                break;
            } else if (parseStack.top() != PAIR) {
                // NOTE: 错误
                return -3;
            }
            // * + 重复
            // TODO: 接入上一个Frag中
            debugPrint(&fragStack.top());
            break;
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
            Frag &tempFrag = fragStack.top();
            tempFrag.last = tempFrag.start;
            break;
        }

        case '\\': {
        }

        default:
            // 正常字符
            cout << "char: " << c << endl;
            switch (parseStack.top()) {
            case BRACKET:
                newValue += c;
                break;
            case PAIR:
            case TRANSFER:
            case NORMAL: {
                fragStack.top().appendNode(c);
                break;
            }
            default:
                assert(!"State is not exist!");
            }
            break;
        }
    }
    // debugPrint(&fragStack.top());
    parseStack.pop();

    // 栈不为空,语法有错误
    if (!parseStack.empty() || fragStack.size() != 1) {
        assert(!"stack not empty!");
        return -2;
    }
    return 0;
}

void NFA::debugPrint(Frag *frag)
{
    typedef vector<Edge>::iterator It;
    State *state;
    stack<State *> stack;
    map<State *, int> record;
    int seq = 0;

    cerr << "digraph NFA {" << endl;
    cerr << "    randir=LR;" << endl;

    // 节点
    state = frag->start;
    record[state] = seq++;
    stack.push(state);
    while (!stack.empty()) {
        state = stack.top();
        stack.pop();
        for (It it = state->vec.begin(); it != state->vec.end(); ++it) {
            cerr << "    ";
            if (!it->next) {
                cerr << "[!] Error Edge" << it->value;
                assert(!"[!] Error Edge");
            }

            // 当前节点
            cerr << record[state];
            cerr << " -> ";
            // 指向节点
            map<State *, int>::iterator f = record.find(it->next);
            if (f == record.end()) {
                record[it->next] = seq++;
            }
            cerr << record[it->next];
            cerr << " [label=\"";
            if (it->value.empty()) {
                cerr << "<empty>";
            } else {
                cerr << it->value;
            }
            cerr << "\"];" << endl;

            stack.push(it->next);
        }
    }

    cerr << "}" << endl;
}

NFA::Edge *NFA::State::addEdge(const std::string &value, State *next)
{
    Edge edge;
    edge.value = value;
    edge.next  = next;
    vec.push_back(edge);
    return &vec[vec.size() - 1];
}

NFA::Edge *NFA::State::addEdge(const char c, State *next)
{
    Edge edge;
    edge.value += c;
    edge.next  = next;
    vec.push_back(edge);
    return &vec[vec.size() - 1];
}

NFA::Edge *NFA::State::addEdge(Edge &edge)
{
    vec.push_back(edge);
    return &vec[vec.size() - 1];
}

void NFA::Frag::appendNode(const char c)
{
    string v = "";
    v += c;
    appendNode(v);
}

void NFA::Frag::appendNode(const std::string &value)
{
    Edge  *edge;
    State *tempState = new State();

    tempState->addEdge("", end);

    /**
     * 对指向end节点的空边进行替换，其余的则是增加一条边
     */
    if (last->vec.size() == 0) {
        last->addEdge(value, tempState);
    } else {
        edge = &last->vec[last->vec.size() - 1];
        if (edge->value == "" && edge->next == end) {
            edge->value = value;
            edge->next  = tempState;
        } else {
            last->addEdge(value, tempState);
        }
    }
    last = tempState;
}
