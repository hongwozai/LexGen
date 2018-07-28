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

class NFA
{
public:

    NFA() {}

    ~NFA() {}

    /**
     * init
     * 初始化两个bigState
     */
    int init();

    /**
     * read
     * @param str 输入的字符串
     * @return
     */
    int read(const char *str, size_t len);

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

        Edge() : value(), next(0) {}
    };

    struct State {
        std::vector<Edge> vec;

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

        // 为了复制使用
        Frag() {}

        Frag(State *start, State *end)
            : start(start),
              end(end),
              last(start),
              edge(0),
              secondLast(last)
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

private:

    std::map<int, State*> bigStates;
};

#endif /* NFA_H */
