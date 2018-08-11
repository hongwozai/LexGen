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

int NFA::init(int seq)
{
    // initial bigState
    bigStates.insert(pair<int, State*>(0, new State(0)));

    this->seq = 1;
    this->numStates = 1;

    if (seq > this->seq) {
        this->seq = seq;
    }
    return 0;
}

int NFA::add(int in, int out, string regexp, bool isfinal)
{
    int ret;
    if (in < 0 || out < 0) {
        return -1;
    }
    if (bigStates.find(in) == bigStates.end()) {
        bigStates[in] = new State(in);
        numStates++;
    }
    if (bigStates.find(out) == bigStates.end()) {
        bigStates[out] = new State(out);
        numStates++;
    }
    if (isfinal) {
        endStates.insert(out);
    }

    ret = read(regexp.data(), regexp.size(),
                bigStates[in], bigStates[out]);
    return ret;
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
    fragStack.push(Frag(start, end, this));
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
            fragStack.push(Frag(tempFrag.last, new State(seq++), this));
            fragStack.top().numStates++;

            bigStates[fragStack.top().end->seq] = fragStack.top().end;
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

            tempFrag.end->addEdge("", fragStack.top().end);
            fragStack.top().last = tempFrag.end;
            fragStack.top().numStates += tempFrag.numStates;

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
                tempFrag.edge->parse();
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
                // 需要添加一个新节点，来让last仅有一个
                tempFrag.secondLast->addEdge("", tempFrag.last);
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
                parseStack.pop();
                fragStack
                    .top()
                    .appendNode(transfer(c, parseStack.top()));
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
    // debugPrint(&fragStack.top());
    numStates += fragStack.top().numStates;
    // cout << "num: " << numStates << endl;
    parseStack.pop();

    // 栈不为空,语法有错误
    if (!parseStack.empty() || fragStack.size() != 1) {
        assert(!"stack not empty!");
        return -2;
    }
    return 0;
}

void NFA::debugPrint()
{
    Frag frag;
    frag.start = bigStates[0];
    debugPrint(&frag);
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
            // cerr << record[state];
            cerr << state->seq;
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

            cerr << it->next->seq;
            // cerr << record[it->next];
            cerr << " [label=\"";
            if (it->value.empty()) {
                cerr << "<empty>";
            } else {
                cerr << it->value;
            }
            cerr << "\"];" << endl;

        }
    }

    // 终结状态
    for (set<int>::iterator it = endStates.begin();
         it != endStates.end();
         ++it) {
        cerr << "    ";
        cerr << *it << " [shape=\"doublecircle\"];" << endl;
    }

    cerr << "}" << endl;
}

NFA::Edge *NFA::State::addEdge(const std::string &value, State *next)
{
    Edge edge;
    edge.value = value;
    edge.next  = next;
    vec.push_back(edge);
    vec[vec.size() - 1].parse();
    return &vec[vec.size() - 1];
}

NFA::Edge *NFA::State::addEdge(const char c, State *next)
{
    Edge edge;
    edge.value += c;
    edge.next  = next;
    vec.push_back(edge);
    vec[vec.size() - 1].parse();
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
    Edge  *edge = NULL;
    State *tempState = new State(nfa->seq++);
    // cout << "state: " << seq - 1 << endl;

    tempState->addEdge("", end);

    /**
     * 对指向end节点的空边进行替换，其余的则是增加一条边
     */
    if (last->vec.size() == 0) {
        edge = last->addEdge(value, tempState);
    } else {
        // 选取最后这个空边且只向终结状态的节点
        bool flag = false;
        for (vector<Edge>::iterator it = last->vec.begin();
             it != last->vec.end();
             ++it) {
            if (it->value.empty() && it->next == end) {
                edge = &*it;
                edge->value = value;
                edge->next  = tempState;
                edge->parse();

                // 找到只向终结状态的空边
                flag = true;
            }
        }
        if (flag == false) {
            edge = last->addEdge(value, tempState);
        }
        assert(edge != NULL);
    }
    this->edge = edge;
    secondLast = last;
    last = tempState;

    numStates += 1;
    nfa->bigStates[tempState->seq] = tempState;
}

int NFA::Edge::parse()
{
    bool isreverse = false;
    bool temp[256] = {false};
    int num = 0;

    if (value.empty()) {
        return 0;
    }
    if (value.size() == 1) {
        if (value[0] == '.') {
            for (int i = 0; i < 256; i++) {
                if (i != '\n')
                    val[i] = true;
            }
            return 255;
        } else {
            val[(int)value[0]] = true;
            return 1;
        }
    }
    if (value[0] != '[' || value[value.size() - 1] != ']') {
        return -1;
    }
    // 判断是否反向
    if (value[1] == '^') {
        isreverse = true;
    }
    // 解析具体的值
    for (size_t i = isreverse?2:1; i < value.size() - 1; i++) {
        if (value[i-1] != '[' &&
            value[i]   == '-'   &&
            value[i+1] != ']') {
            for (int j = value[i-1] + 1; j <= value[i+1]; j++) {
                temp[j] = true;
            }
            i++;
        } else {
            temp[(int)value[i]] = true;
        }
    }
    // 根据是否反向进行
    for (int i = 0; i < 256; i++) {
        // if (temp[i] == true && isreverse == false ||
        //     temp[i] == false && isreverse == true) {
        if (temp[i] ^ isreverse) {
            val[i] = true;
            num++;
        }
    }
    return num;
}

string NFA::transfer(char c, int env)
{
    if (env == BRACKET) {
        return string(c, 1);
    }
    switch(c) {
    case 'd':
        return string("[0-9]");
    case 'D':
        return string("[^0-9]");
    case 's':
        // space tab
        return string("[ \t\v\n\r\f]");
    case 'S':
        return string("[^ \t\v\n\r\f]");
    case 'w':
        return string("[_0-9A-Za-z]");
    case 'W':
        return string("[^_0-9A-Za-z]");
    default:
        return string(c, 1);
    }
}