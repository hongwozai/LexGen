
#include <cstdio>
struct Res {
    // token type
    int type;
    // token len
    int size;
};

int match(const char *str, int len, Res *r)
{
    int  curr = 0;
    int  lastType = 0;
    int  lastFinal = -1;

    curr = 1;
    for (int i = 0; i < len; i++) {
        char c = str[i];

        // printf("curr: %d, c: %c, %x, %d\n", curr, c, c, c);
        curr = stateTable[curr][(unsigned)c];
        if (curr == 0) {
            // error or finish
            break;
        }
        if (stateTable[curr][256] > 0) {
            lastFinal = i;
            lastType  = stateTable[curr][256];
        }
    }

    // match failure
    if (lastFinal == -1) {
        if (len <= 0)
            return -1;
        return -2;
    }
    // match success
    if (r) {
        r->size = lastFinal + 1;
        r->type = lastType;
    }
    return 0;
}

#include <string>
#include <cstdio>
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    int i = 0, ret = 0;
    Res res;
    std::string str = ";100e11 100 +100 -100 45e11 45e+11 .21 .21e11 .21e+11 .21e-11 0.3434\n 2345234.123e11   word +1a23";

    while (0 == (ret = match(str.data() + i, str.size() - i, &res))) {
        cout << "res.type: " << res.type << endl;
        cout << "res.size: " << res.size << endl;
        fprintf(stdout, "word: %.*s\n", res.size, str.data() + i);
        i += res.size;
    }
    if (ret < 0) {
        if (ret == -1) {
            cout << "finished!\n" << endl;
            return 0;
        }
        cout << "pos: " << i + 1
             << ": match failure!" << endl;
    }
    return 0;
}