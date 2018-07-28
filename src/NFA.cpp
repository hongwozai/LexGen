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

/**
 * 核心思想：
 * parseStack解析栈
 * fragStack capture栈
 */
int NFA::read(const char *str, size_t len, State *start, State *end)
{
    string          newValue;
    stack<Frag>     fragStack;
    stack<int>      parseStack;

    // 记录最后一个是capture还是单宽度的字符
    Frag lastFrag;
    bool isCapture = false;

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
            isCapture = false;
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
            fragStack.push(Frag(tempFrag.last, new State()));
            // NOTE: 注意这里最后一条边在capture之前
            break;
        }

        case ')': {
            Frag &tempFrag = fragStack.top();
            fragStack.pop();
            if (parseStack.top() == BRACKET) {
                newValue += c;
                break;
            } else if (parseStack.top() == TRANSFER) {
                fragStack.top().appendNode(c);
                break;
            } else if (parseStack.top() != PAIR) {
                return -3;
            }
            parseStack.pop();
            // TODO: * + 重复
            tempFrag.end->addEdge("", fragStack.top().end);
            fragStack.top().last = tempFrag.end;

            // 记录最后一个capture，以便可以*+?
            isCapture = true;
            lastFrag = tempFrag;
            // debugPrint(&fragStack.top());
            break;
        }

        case '*': {
            Frag &tempFrag = fragStack.top();
            if (parseStack.top() == BRACKET) {
                newValue += c;
                break;
            } else if (parseStack.top() == TRANSFER) {
                fragStack.top().appendNode(c);
                break;
            } else if (tempFrag.last == tempFrag.start) {
                return -4;
            }
            if (isCapture) {
                // +与?的集合体
                lastFrag.end->addEdge("", lastFrag.start);
                lastFrag.start->addEdge("", lastFrag.end);
            } else {
                tempFrag.last->addEdge(tempFrag.edge->value, tempFrag.last);
                tempFrag.edge->value = "";
            }
            break;
        }

        case '+': {
            Frag &tempFrag = fragStack.top();
            if (parseStack.top() == BRACKET) {
                newValue += c;
                break;
            } else if (parseStack.top() == TRANSFER) {
                fragStack.top().appendNode(c);
                break;
            } else if (tempFrag.last == tempFrag.start) {
                return -5;
            }
            if (isCapture) {
                // 尾部直接指向头部即可
                lastFrag.end->addEdge("", lastFrag.start);
            } else {
                tempFrag.last->addEdge(tempFrag.edge->value, tempFrag.last);
            }
            break;
        }

        case '?': {
            Frag &tempFrag = fragStack.top();
            if (parseStack.top() == BRACKET) {
                newValue += c;
                break;
            } else if (parseStack.top() == TRANSFER) {
                fragStack.top().appendNode(c);
                break;
            } else if (tempFrag.last == tempFrag.start) {
                return -6;
            }
            if (isCapture) {
                lastFrag.start->addEdge("", lastFrag.end);
            } else {
                tempFrag.secondLast->addEdge("", tempFrag.end);
            }
            break;
        }

        case '|': {
            Frag &tempFrag = fragStack.top();
            tempFrag.edge = NULL;
            tempFrag.last = tempFrag.start;
            break;
        }

        case '\\': {
            parseStack.push(TRANSFER);
            break;
        }

        case '.': {
            // 任意字符，到下一个步骤中处理
            // NOTE: continue;
        }

        default:
            // 正常字符
            // cout << "char: " << c << endl;
            switch (parseStack.top()) {
            case BRACKET:
                newValue += c;
                break;
            case TRANSFER:
                fragStack.top().appendNode(c);
                parseStack.pop();
                break;
            case PAIR:
                // continue
            case NORMAL: {
                fragStack.top().appendNode(c);
                break;
            }
            default:
                assert(!"State is not exist!");
            }
            isCapture = false;
            break;
        }
    }
    debugPrint(&fragStack.top());
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

                // 没访问过的(没有分配过序号)
                stack.push(it->next);
            } else {
                // 访问过的不再访问
            }

            cerr << record[it->next];
            cerr << " [label=\"";
            if (it->value.empty()) {
                cerr << "<empty>";
            } else {
                cerr << it->value;
            }
            cerr << "\"];" << endl;

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
        edge = last->addEdge(value, tempState);
    } else {
        edge = &last->vec[last->vec.size() - 1];
        // NOTE: 此处认为最后节点的空边指向终结状态
        //       该函数本身也只对最后节点使用，
        //       而最后节点要么只有空边，要么没有边
        if (edge->value == "") {
            edge->value = value;
            edge->next  = tempState;
        } else {
            edge = last->addEdge(value, tempState);
        }
    }
    this->edge = edge;
    secondLast = last;
    last = tempState;
}
