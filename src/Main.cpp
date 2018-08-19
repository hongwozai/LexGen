#include "LexGen.h"

#include <cctype>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void splitString(string &str, vector<string> &vec, int sep)
{
    string s;

    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == sep) {
            vec.push_back(s);
            s = "";
            continue;
        }
        s += str[i];
    }
    vec.push_back(s);
}

template <class T>
T fromStr(string &str)
{
    stringstream ss;
    T t;
    ss << str;
    ss >> t;
    return t;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "args error!" << endl;
        return 0;
    }

    ifstream file(argv[1]);
    string   line;

    vector<LexGen::Conf> confs;
    while (getline(file, line)) {
        LexGen::Conf   conf;
        vector<string> strConf;

        // 放松语法
        if (line.empty() || line[0] == '#' || isspace(line[0])) {
            continue;
        }

        splitString(line, strConf, '\t');
        if (strConf.size() != 4) {
            cerr << "format error("
                 << strConf.size()
                 << ")!" << endl;
            return -2;
        }

        conf.start = fromStr<int>(strConf[0]);
        conf.end   = fromStr<int>(strConf[1]);
        conf.regexp = strConf[2];
        conf.isfinish = fromStr<bool>(strConf[3]);

        confs.push_back(conf);

        line = "";
    }

#if 0
        cerr << "conf size: " << confs.size() << endl;
        for (size_t i = 0; i < confs.size(); i++) {
            cerr << confs[i].start << ",";
            cerr << confs[i].end << ",";
            cerr << "\"" << confs[i].regexp << "\",";
            cerr << (confs[i].isfinish ? "true" : "false");
            cerr << endl;
        }
#endif
    {
        LexGen lex;

        if (lex.init(confs.data(), confs.size()) < 0) {
            cout << "stop parse!" << endl;
            return -1;
        }
        lex.printTable();
        cout << endl;
        lex.printCode();
    }
    return 0;
}
