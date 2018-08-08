/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：NFA.h
 ** 创建人：zeya
 ** 创建日期： 2018-07-21
 **
 ** 描  述：NFA生成
 ** 注  意：
 **************************************************/
#ifndef NFA_H
#define NFA_H

#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <set>

class NFA
{
public:

    NFA() : seq(0) {
        init();
    }

    ~NFA() {}

    /**
     * init
     * 初始化两个bigState
     */
    int init(int seq = 2);

    /**
     * 添加正则与状态
     * @param isfinal out节点是否为终结节点
     */
    int add(int in, int out, std::string regexp,
            bool isfinal = true);

    /**
     * debugPrint
     * 打印dot语法的状态转换图
     */
    void debugPrint();

private:

    struct State;

    struct Edge {
        std::string value;
        State *next;
        // val是value解析出来的具体值
        bool        val[256];

        Edge() : value(), next(0) {
            memset(val, 0, sizeof(bool) * 256);
        }

        /**
         * 解析每条边的输入
         * 任何value的改变都需要调用该函数
         */
        int parse();

        void setValue(std::string &value) {
            this->value = value;
            parse();
        }
    };

    struct State {
        int seq;
        std::vector<Edge> vec;

        explicit State() : seq(0) {}

        explicit State(int seq) : seq(seq) {}

        Edge *addEdge(const std::string &, State *);
        Edge *addEdge(const char, State *);
        Edge *addEdge(Edge &edge);
    };

    struct Frag {
        // 起始状态和终结状态
        State *start;
        State *end;

        // 最后一个状态（除终结状态外）
        State *last;
        // 到达最后一个状态的边
        Edge  *edge;
        // 倒数第二个状态
        State *secondLast;

        // 统计该frag有多少节点
        int numStates;

        // 序号
        NFA *nfa;

        // 为了复制使用
        Frag() {}

        Frag(State *start, State *end, NFA *nfa)
            : start(start),
              end(end),
              last(start),
              edge(0),
              secondLast(last),
              numStates(0),
              nfa(nfa)
        {}

        void appendNode(const char c);
        void appendNode(const std::string &);
    };

    // only use in parseStack
    enum {
        NORMAL = 0,
        BRACKET,                // []
        PAIR,                   // ()
        TRANSFER,               // '\'
    };

private:

    int read(const char *str, size_t len, State *state, State *end);

    void debugPrint(Frag *frag);

    std::string transfer(char c, int env);

public:

    std::map<int, State*> bigStates;
    std::set<int>         endStates;

    int numStates;

private:

    /**
     * 该序号为状态序号，自增使用，独一无二
     */
    int seq;

private:

    friend class DFA;
};

#endif /* NFA_H */
