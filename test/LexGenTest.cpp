#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "NFA.h"
#include "DFA.h"
#include "LexGen.h"
using namespace std;

enum {
    START = 0,
    END,
    TAG,
    WORD,

    TAG_INTERNAL,
};

TEST(lexgen, a)
{
    int ret;
    LexGen::Res res;
    LexGen lexgen;

    LexGen::Conf array[] = {
        {0, 1, "\\w+", true},
        {0, 2, "<[^>]+>", true}
    };

    lexgen.init(array, 2);

    string html = "word<html>";
    ret = lexgen.match(html.data(), html.size(), &res);
    // lexgen.printTable();
    // lexgen.printCode();

    if (ret == -1) {
        cout << "match failure" << endl;
    } else {
        cout << "res.type: " << res.type << endl;
        cout << "res.size: " << res.size << endl;
        cout << string(html.data(), res.size) << endl;
    }
}
